#include "./tasks/task_can_rx.h"

#include <stdio.h>
#include <stdlib.h>   // abs
#include "i2c.h"     // GPIOF, hi2c1, htim1, HAL_GetTick, HAL_GPIO_WritePin
#include "tim.h"
#include "sys_helpers.h"  // sys_log
#include "can_id.h"      // CAN_ID_*
#include "can_tx.h"      // mcp_send_message()

#include "motor_control.h"
#include "servo.h"

// ---- Timing constants (keep original behavior) ----
#ifndef CAN_PERIOD_MOTOR_STATUS_MS
#define CAN_PERIOD_MOTOR_STATUS_MS 100  // adjust if already defined elsewhere
#endif

enum {
  CAN_RX_SLEEP_TICKS = 10,      // was tx_thread_sleep(10)
  CAN_RX_MAX_FRAMES_PER_STEP = 16  // bound work per step for determinism
};

typedef struct
{
  HAL_StatusTypeDef motor_init_status;
  HAL_StatusTypeDef servo_init_status;

  uint32_t last_motor_status_tick;
  uint32_t last_debug_tick;

  int8_t   actual_throttle_applied;
  int8_t   actual_steering_applied;
  uint16_t motor_current_estimate_ma;
  uint8_t  motor_status_counter;
  DriveMode_t current_drive_mode;

  uint8_t  cmd_log_counter;
  uint8_t  joystick_log_counter;
} TaskCanRx;

static TaskCanRx s_rx;

static void send_motor_status_if_due(void)
{
  uint32_t now = HAL_GetTick();
  if ((now - s_rx.last_motor_status_tick) < CAN_PERIOD_MOTOR_STATUS_MS)
    return;

  s_rx.last_motor_status_tick = now;

  MotorStatus_t motor_status_frame;
  motor_status_frame.actual_throttle   = s_rx.actual_throttle_applied;
  motor_status_frame.actual_steering   = s_rx.actual_steering_applied;
  motor_status_frame.motor_current_ma  = s_rx.motor_current_estimate_ma;
  motor_status_frame.driver_temp       = 25; // TODO real
  motor_status_frame.pwm_duty          = (uint8_t)((abs(s_rx.actual_throttle_applied) * 255) / 100);
  motor_status_frame.counter           = s_rx.motor_status_counter++;

  motor_status_frame.crc = calculate_crc8((uint8_t*)&motor_status_frame,
                                         sizeof(motor_status_frame) - 1);

  mcp_send_message(CAN_ID_MOTOR_STATUS, (uint8_t*)&motor_status_frame,
                   sizeof(motor_status_frame));
}

static void clear_mcp_flags_if_due(void)
{
  uint32_t now = HAL_GetTick();
  if ((now - s_rx.last_debug_tick) < 5000u)
    return;

  s_rx.last_debug_tick = now;

  uint8_t canintf = MCP2515_ReadRegister(REG_CANINTF);

  uint8_t error_flags = (uint8_t)(canintf & 0xFC); // bits 2-7
  if (error_flags)
    MCP2515_BitModify(REG_CANINTF, error_flags, 0x00);

  uint8_t eflg = MCP2515_ReadRegister(REG_EFLG);
  if (eflg != 0x00)
    MCP2515_WriteRegister(REG_EFLG, 0x00);
}

static void handle_emergency_stop(SystemCtx* ctx, const CAN_Message_t* rx_msg, const VehicleState* snap)
{
  if (rx_msg->dlc < sizeof(EmergencyStop_t))
    return;

  if (!validate_crc8(rx_msg->data, sizeof(EmergencyStop_t)))
  {
    sys_log(ctx, "\033[1;31m[CAN_RX] EmergencyStop CRC INVÁLIDO!\033[0m");
    return;
  }

  const EmergencyStop_t* estop = (const EmergencyStop_t*)rx_msg->data;

  if (estop->active)
  {
    tx_mutex_get(&ctx->state_mutex, TX_WAIT_FOREVER);
    ctx->state.emergency_stop_active = 1;
    tx_mutex_put(&ctx->state_mutex);

    Motor_Stop();
    s_rx.actual_throttle_applied = 0;
    s_rx.actual_steering_applied = 0;

    sys_log(ctx,
      "\033[1;31m[CAN_RX] EMERGENCY STOP from AGL! Source=%u Reason=0x%02X Dist=%u mm\033[0m",
      estop->source, estop->reason, estop->distance_mm
    );
  }
  else
  {
    tx_mutex_get(&ctx->state_mutex, TX_WAIT_FOREVER);
    ctx->state.emergency_stop_active = 0;
    tx_mutex_put(&ctx->state_mutex);
    sys_log(ctx, "\033[1;32m[CAN_RX] Emergency CLEARED by AGL\033[0m");
  }
}

static void handle_motor_cmd(SystemCtx* ctx, const CAN_Message_t* rx_msg, const VehicleState* snap)
{
  if (rx_msg->dlc < sizeof(MotorCmd_t))
    return;

  if (!validate_crc8(rx_msg->data, sizeof(MotorCmd_t)))
  {
    sys_log(ctx, "\033[1;31m[CAN_RX] MotorCmd CRC INVÁLIDO!\033[0m");
    return;
  }

  const MotorCmd_t* cmd = (const MotorCmd_t*)rx_msg->data;

  if (cmd->flags & CMD_FLAG_EMERGENCY_STOP)
  {
    Motor_Stop();
    s_rx.actual_throttle_applied = 0;
    s_rx.actual_steering_applied = 0;
    sys_log(ctx, "\033[1;31m[CAN_RX] EMERGENCY STOP!\033[0m");
    return;
  }

  int8_t throttle = cmd->throttle;

  // Block forward during emergency stop (reverse OK)
  if (snap->emergency_stop_active && throttle >= 0)
  {
    sys_log(ctx, "\033[1;33m[CAN_RX] Forward BLOCKED - Emergency! (Reverse OK)\033[0m");
    Motor_Stop();
    s_rx.actual_throttle_applied = 0;
    return;
  }

  s_rx.current_drive_mode = (DriveMode_t)cmd->mode;

  int8_t steering = cmd->steering;
  if (steering < -100) steering = -100;
  if (steering > 100)  steering = 100;

  uint8_t servo_angle = (uint8_t)((steering + 100) * 180 / 200);
  Servo_SetAngle(servo_angle);
  s_rx.actual_steering_applied = steering;

  if (throttle < -100) throttle = -100;
  if (throttle > 100)  throttle = 100;

  // Apply SRF08 speed limit (only clamps positive throttle)
  if (throttle > (int8_t)snap->srf08_speed_limit)
    throttle = (int8_t)snap->srf08_speed_limit;

  if ((cmd->flags & CMD_FLAG_BRAKE) || throttle == 0)
  {
    Motor_Stop();
    s_rx.actual_throttle_applied = 0;
  }
  else if (throttle > 0)
  {
    Motor_Forward((uint8_t)throttle);
    s_rx.actual_throttle_applied = throttle;
  }
  else
  {
    Motor_Backward((uint8_t)(-throttle));
    s_rx.actual_throttle_applied = throttle;
  }

  s_rx.motor_current_estimate_ma = (uint16_t)(abs(throttle) * 20); // ~2A @ 100%

  if (++s_rx.cmd_log_counter >= 10)
  {
    s_rx.cmd_log_counter = 0;
    sys_log(ctx,
      "\033[1;32m[CAN_RX] MotorCmd: T=%d S=%d Mode=%u Flags=0x%02X\033[0m",
      throttle, steering, cmd->mode, cmd->flags
    );
  }
}

static void handle_config_cmd(SystemCtx* ctx, const VehicleState* snap)
{
  sys_log(ctx, "\033[1;33m[CAN_RX] ConfigCmd recebido (não implementado)\033[0m");
}

static void handle_heartbeat_agl(SystemCtx* ctx, const CAN_Message_t* rx_msg, const VehicleState* snap)
{
  if (rx_msg->dlc < sizeof(Heartbeat_t))
    return;

  if (!validate_crc8(rx_msg->data, sizeof(Heartbeat_t)))
  {
    sys_log(ctx, "\033[1;31m[CAN_RX] Heartbeat AGL CRC INVÁLIDO!\033[0m");
    return;
  }

  const Heartbeat_t* hb = (const Heartbeat_t*)rx_msg->data;

  sys_log(ctx,
    "\033[1;32m[CAN_RX] Heartbeat AGL: State=%u Uptime=%lu ms Errors=0x%02X\033[0m",
    hb->state, hb->uptime_ms, hb->errors
  );

  // TODO watchdog timeout logic
}

static void handle_relay_cmd(SystemCtx* ctx, const CAN_Message_t* rx_msg, const VehicleState* snap)
{
  if (rx_msg->dlc < 1)
    return;

  uint8_t relay_state = rx_msg->data[0];

  if (relay_state == 1)
  {
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_SET);
    sys_log(ctx, "\033[1;32m[CAN_RX] RELAY ON\033[0m");
  }
  else
  {
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_RESET);
    sys_log(ctx, "\033[1;31m[CAN_RX] RELAY OFF\033[0m");
  }
}

static void handle_joystick(SystemCtx* ctx, const CAN_Message_t* rx_msg, const VehicleState* snap)
{
  if (rx_msg->dlc < 4)
    return;

  int16_t steering = (int16_t)(rx_msg->data[0] | (rx_msg->data[1] << 8));
  int16_t throttle = (int16_t)(rx_msg->data[2] | (rx_msg->data[3] << 8));

  if (snap->emergency_stop_active && throttle >= 0)
  {
    sys_log(ctx, "\033[1;33m[CAN_RX] Joystick Forward BLOCKED - Emergency! (Reverse OK)\033[0m");
    Motor_Stop();
    s_rx.actual_throttle_applied = 0;
    return;
  }

  if (steering < -100) steering = -100;
  if (steering > 100)  steering = 100;
  if (throttle < -100) throttle = -100;
  if (throttle > 100)  throttle = 100;

  if (throttle > (int8_t)snap->srf08_speed_limit)
    throttle = (int8_t)snap->srf08_speed_limit;

  uint8_t servo_angle = (uint8_t)((steering + 100) * 180 / 200);
  Servo_SetAngle(servo_angle);
  s_rx.actual_steering_applied = (int8_t)steering;

  if (throttle > 10)
  {
    Motor_Forward((uint8_t)throttle);
    s_rx.actual_throttle_applied = (int8_t)throttle;
  }
  else if (throttle < -10)
  {
    Motor_Backward((uint8_t)(-throttle));
    s_rx.actual_throttle_applied = (int8_t)throttle;
  }
  else
  {
    Motor_Stop();
    s_rx.actual_throttle_applied = 0;
  }

  if (++s_rx.joystick_log_counter >= 10)
  {
    s_rx.joystick_log_counter = 0;
    sys_log(ctx,
      "\033[1;34m[CAN_RX] Joystick: S=%d (%u°) T=%d\033[0m",
      (int)steering, (unsigned)servo_angle, (int)throttle
    );
  }
}

static void process_one_rx(SystemCtx* ctx, const CAN_Message_t* rx_msg, const VehicleState* snap)
{
  switch (rx_msg->id)
  {
    case CAN_ID_EMERGENCY_STOP: handle_emergency_stop(ctx, rx_msg, snap); break;
    case CAN_ID_MOTOR_CMD:      handle_motor_cmd(ctx, rx_msg, snap);      break;
    case CAN_ID_CONFIG_CMD:     handle_config_cmd(ctx, snap);            break;
    case CAN_ID_HEARTBEAT_AGL:  handle_heartbeat_agl(ctx, rx_msg, snap);  break;
    case CAN_ID_CMD_RELAY:      handle_relay_cmd(ctx, rx_msg, snap);      break;
    case CAN_ID_JOYSTICK:       handle_joystick(ctx, rx_msg, snap);       break;

    default:
      sys_log(ctx, "\033[1;36m[CAN_RX] ID desconhecido: 0x%03lX\033[0m", rx_msg->id);
      break;
  }
}

void task_can_rx_init(SystemCtx* ctx)
{
  // deterministic reset
  TaskCanRx z = {0};
  s_rx = z;

  // Safe relay init
  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_RESET);

  sys_log(ctx, "\033[1;36m[CAN_RX] Thread iniciada - Protocolo CAN atualizado!\033[0m");

  s_rx.motor_init_status = Motor_Init(ctx, &hi2c1);
  if (s_rx.motor_init_status == HAL_OK)
    sys_log(ctx, "\033[1;32m[CAN_RX] Motor Driver TB6612FNG inicializado!\033[0m");
  else
    sys_log(ctx, "\033[1;31m[CAN_RX] ERRO ao inicializar Motor Driver! Status: %d\033[0m",
            s_rx.motor_init_status);

  s_rx.servo_init_status = Servo_Init(&htim1, TIM_CHANNEL_1);
  if (s_rx.servo_init_status == HAL_OK)
    sys_log(ctx, "\033[1;32m[CAN_RX] Servo MG996R inicializado (PA8/TIM1_CH1)!\033[0m");
  else
    sys_log(ctx, "\033[1;31m[CAN_RX] ERRO ao inicializar Servo! Status: %d\033[0m",
            s_rx.servo_init_status);

  sys_log(ctx, "\033[1;36m[CAN_RX] Aguardando comandos (0x200, 0x201, 0x500, 0x700, 0x801)...\033[0m");
}

void task_can_rx_step(SystemCtx* ctx)
{
  // periodic work (same behavior)
  send_motor_status_if_due();
  clear_mcp_flags_if_due();

  //get the latest state snapshot for decision making
  VehicleState snap;

  tx_mutex_get(&ctx->state_mutex, TX_WAIT_FOREVER);
  snap = ctx->state;
  tx_mutex_put(&ctx->state_mutex);

  // RX drain with bounded work
  CAN_Message_t rx_msg;
  uint32_t processed = 0;

  while (processed < CAN_RX_MAX_FRAMES_PER_STEP)
  {
    uint8_t has_msg = MCP2515_CheckReceive();
    if (!has_msg)
      break;

    if (MCP2515_ReadMessage(&rx_msg))
    {
      process_one_rx(ctx, &rx_msg, &snap);
      processed++;
    }
    else
    {
      // if read fails, break to avoid spinning
      break;
    }
  }

  // Small delay to avoid busy waiting (same)
  tx_thread_sleep(CAN_RX_SLEEP_TICKS);
}
