#include "./tasks/task_can_rx.h"
#include "./tasks/task_indicator.h"
#include "./tasks/task_cruise_control.h"
#include "./tasks/task_aeb.h"

#include <stdio.h>
#include <stdlib.h>   // abs
#include "i2c.h"     // GPIOF, hi2c1, htim1, HAL_GetTick, HAL_GPIO_WritePin
#include "tim.h"
#include "sys_helpers.h"  // sys_log
#include "can_id.h"      // CAN_ID_*
#include "can_protocol.h" // CtrlCmd_t, CAN_ID_CTRL_CMD
#include "can_tx.h"      // mcp_send_message()
#include "stdbool.h"
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

// Global maximum throttle cap: limits top speed regardless of AEB state
#define AEB_MAX_THROTTLE_PCT 100u

/* CtrlCmd (0x202) staleness watchdog: se o Manager deixar de enviar (crash,
 * CAN cortado) com throttle MANUAL/LKA aplicado, mais nada pára o motor —
 * o staleness de 200ms do CC só cobre CC/ACC. */
#define CTRL_CMD_STALE_MS 300u

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
  uint32_t last_ctrl_cmd_tick;   /* HAL_GetTick() do último 0x202 válido — 0 = nunca */
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

static void clear_mcp_flags_if_due(SystemCtx* ctx)
{
  uint32_t now = HAL_GetTick();

  /* Bus-off check runs every 500 ms (was 5000 ms).
   * When the RPi Manager stops, its CAN interface goes down and stops
   * sending ACKs.  The MCP2515 TEC reaches 255 quickly (~128 frames × ~20 ms
   * = ~2.5 s) and enters bus-off.  A 5 s poll meant the STM32 was silent for
   * up to 7.5 s after the Manager restarted — forcing a manual Reset.
   * 500 ms recovery means the MCP2515 is back on the bus within ~1 s. */
  if ((now - s_rx.last_debug_tick) < 500u)
    return;

  s_rx.last_debug_tick = now;

  tx_mutex_get(&ctx->spi1_mutex, TX_WAIT_FOREVER);

  uint8_t canintf = MCP2515_ReadRegister(REG_CANINTF);
  uint8_t error_flags = (uint8_t)(canintf & 0xFC); // bits 2-7
  if (error_flags)
    MCP2515_BitModify(REG_CANINTF, error_flags, 0x00);

  uint8_t eflg = MCP2515_ReadRegister(REG_EFLG);
  if (eflg != 0x00)
    MCP2515_WriteRegister(REG_EFLG, 0x00);

  tx_mutex_put(&ctx->spi1_mutex);

  /* TXBO (bit 5): bus-off — TEC reached 255, MCP2515 stops all TX/RX.
   * Recovery: config mode (resets TEC/REC) → normal mode.
   * Happens when RPi CAN interface goes down (no ACK) during Manager restart. */
  if (eflg & 0x20)
  {
    sys_log(ctx, "\033[1;31m[CAN_RX] BUS-OFF detectado (EFLG=0x%02X)"
                 " — a recuperar MCP2515...\033[0m", eflg);

    tx_mutex_get(&ctx->spi1_mutex, TX_WAIT_FOREVER);
    MCP2515_WriteRegister(REG_CANCTRL, 0x80); /* → config mode: resets TEC/REC */
    tx_mutex_put(&ctx->spi1_mutex);

    tx_thread_sleep(5);

    tx_mutex_get(&ctx->spi1_mutex, TX_WAIT_FOREVER);
    MCP2515_WriteRegister(REG_CANCTRL, 0x00); /* → normal mode */
    tx_mutex_put(&ctx->spi1_mutex);

    tx_thread_sleep(5);

    sys_log(ctx, "\033[1;32m[CAN_RX] MCP2515 recuperado de bus-off\033[0m");
  }
}

/*
 * Watchdog timeout para emergency_stop_active.
 *
 * Problema: o Manager (AGL/RPi) envia EmergencyStop(active=1) UMA vez quando
 * entra em EMERGENCY_STOP, e EmergencyStop(active=0) UMA vez ao sair.  Se a
 * frame de clear (active=0) for perdida no barramento CAN (bus-off transitório,
 * MCP2515 ocupado a TX, race a aceder ao SPI, etc.), o STM32 fica preso com
 * emergency_stop_active=1 indefinidamente — toda a marcha à frente bloqueada,
 * sem forma de recuperar excepto reset físico.
 *
 * Solução: tratar emergency_stop_active como soft-latch com auto-expiry.
 * Quando o Manager activa, marca timestamp; se passarem mais de N ms sem
 * nova confirmação (frame active=1), assume que o Manager já clareou e
 * a frame foi perdida.  Auto-limpa o flag e re-inicia o AEB local.
 *
 * Para o Manager realmente manter o estado de emergency, basta re-enviar
 * EmergencyStop(active=1) periodicamente (heartbeat) — qualquer frame
 * reseta o timestamp e prolonga o LATCH.
 *
 * 3000 ms é um compromisso: longo o suficiente para sobreviver a uma frame
 * perdida ocasional, curto o suficiente para o operador não esperar muito
 * tempo se o Manager nunca chegar a enviar o clear.
 */
#define EMERGENCY_STOP_TIMEOUT_MS  3000u

static void check_emergency_stop_timeout(SystemCtx* ctx)
{
  uint8_t  active;
  uint32_t ts;

  tx_mutex_get(&ctx->state_mutex, TX_WAIT_FOREVER);
  active = ctx->state.emergency_stop_active;
  ts     = ctx->state.emergency_stop_ts;
  tx_mutex_put(&ctx->state_mutex);

  if (!active) return;

  uint32_t age = HAL_GetTick() - ts;
  if (age < EMERGENCY_STOP_TIMEOUT_MS) return;

  /* Timeout — assume que o Manager já enviou estop=0 mas perdeu-se.
   * Limpa o flag e desbloqueia o carro (incluindo libertar o AEB latch,
   * por consistência com o caminho explícito acima). */
  tx_mutex_get(&ctx->state_mutex, TX_WAIT_FOREVER);
  ctx->state.emergency_stop_active = 0;
  ctx->state.emergency_stop_ts     = 0;
  tx_mutex_put(&ctx->state_mutex);

  task_aeb_force_reset(ctx);

  tx_mutex_get(&ctx->sys_mutex, TX_WAIT_FOREVER);
  ctx->system_state = SYSTEM_STATE_RUNNING;
  tx_mutex_put(&ctx->sys_mutex);

  task_indicator_set_state(INDICATOR_OFF);

  sys_log(ctx,
    "\033[1;33m[CAN_RX] Emergency TIMEOUT %lus — auto-clear (Manager pode ter perdido"
    " a frame de estop=0)\033[0m",
    (unsigned long)(EMERGENCY_STOP_TIMEOUT_MS / 1000u));
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
    ctx->state.emergency_stop_ts     = HAL_GetTick();  /* arranca watchdog timer */
    tx_mutex_put(&ctx->state_mutex);

    Motor_Stop();
    s_rx.actual_throttle_applied = 0;
    s_rx.actual_steering_applied = 0;

    task_indicator_set_state(INDICATOR_ALERT);
    tx_mutex_get(&ctx->sys_mutex, TX_WAIT_FOREVER);
    ctx->system_state = SYSTEM_STATE_SAFE;
    tx_mutex_put(&ctx->sys_mutex);

    sys_log(ctx,
      "\033[1;31m[CAN_RX] EMERGENCY STOP from AGL! Source=%u Reason=0x%02X Dist=%u mm\033[0m",
      estop->source, estop->reason, estop->distance_mm
    );
  }
  else
  {
    tx_mutex_get(&ctx->state_mutex, TX_WAIT_FOREVER);
    ctx->state.emergency_stop_active = 0;
    ctx->state.emergency_stop_ts     = 0;
    tx_mutex_put(&ctx->state_mutex);

    /* Also release the local AEB latch so the Manager can resume control.
     * The AEB step will immediately re-arm on the next cycle (~20 ms) and
     * re-latch if an obstacle is still present — so this is safe. */
    task_aeb_force_reset(ctx);

    sys_log(ctx, "\033[1;32m[CAN_RX] Emergency CLEARED by AGL — AEB latch também libertado\033[0m");
    task_indicator_set_state(INDICATOR_OFF);
    tx_mutex_get(&ctx->sys_mutex, TX_WAIT_FOREVER);
    ctx->system_state = SYSTEM_STATE_RUNNING;
    tx_mutex_put(&ctx->sys_mutex);
  }
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

  // TODO watchdog timeout logic
}

static void handle_indicator_cmd(SystemCtx* ctx, const CAN_Message_t* rx_msg)
{
    if (rx_msg->dlc < 1)
        return;

    uint8_t raw = rx_msg->data[0];
    if (raw > INDICATOR_ALERT)
    {
        sys_log(ctx, "\033[1;31m[CAN_RX] INDICATOR: estado inválido %u\033[0m", raw);
        return;
    }

    IndicatorState_t state = (IndicatorState_t)raw;
    task_indicator_set_state(state);

    static const char* const names[] = { "OFF", "PISCA_ESQ", "PISCA_DIR", "FAROIS", "ALERTA" };
    (void)names;
}

/* Apply CtrlCmd_t (0x202, CtrlMode_t-driven) to servo + motor.
 *
 * Replaces the old MotorCmd_t (0x200) handler. Steering is always applied.
 * Throttle source depends on mode:
 *   MANUAL / LKA → cmd->throttle directly
 *   CC / ACC     → no speed-PID yet (step 4) → safe stop
 *   DISABLED     → safe stop
 * AEB / E-stop latches keep absolute override regardless of mode.
 */
static void handle_ctrl_cmd(SystemCtx* ctx, const CAN_Message_t* rx_msg,
                            const VehicleState* snap)
{
    if (rx_msg->dlc < sizeof(CtrlCmd_t))
        return;

    if (!validate_crc8(rx_msg->data, sizeof(CtrlCmd_t)))
    {
        sys_log(ctx, "\033[1;31m[CAN_RX] CtrlCmd CRC INVÁLIDO!\033[0m");
        return;
    }

    const CtrlCmd_t* cmd = (const CtrlCmd_t*)rx_msg->data;

    s_rx.last_ctrl_cmd_tick = HAL_GetTick();   /* alimenta o watchdog de staleness */

    /* --- 1. Steering — always applied ----------------------------------- */
    int8_t steering = cmd->steering;
    if (steering < -100) steering = -100;
    if (steering >  100) steering =  100;

    uint8_t servo_angle = (uint8_t)((100 - steering) * 180 / 200);
    Servo_SetAngle(servo_angle);
    s_rx.actual_steering_applied = steering;

    /* --- 2. Mirror mode into legacy DriveMode_t for downstream consumers - */
    DriveMode_t dm;
    switch ((CtrlMode_t)cmd->mode) {
        case CTRL_MODE_MANUAL: dm = DRIVE_MODE_MANUAL;     break;
        case CTRL_MODE_LKA:
        case CTRL_MODE_CC:
        case CTRL_MODE_ACC:    dm = DRIVE_MODE_AUTONOMOUS; break;
        case CTRL_MODE_DISABLED:
        default:               dm = DRIVE_MODE_IDLE;       break;
    }
    s_rx.current_drive_mode = dm;

    tx_mutex_get(&ctx->state_mutex, TX_WAIT_FOREVER);
    ctx->state.drive_mode  = dm;
    ctx->state.servo_angle = steering;
    tx_mutex_put(&ctx->state_mutex);

    /* --- 3. Publish this command to the CC/ACC thread ------------------- */
    bool wants_cc = (cmd->mode == CTRL_MODE_CC || cmd->mode == CTRL_MODE_ACC);

    tx_mutex_get(&g_cc_mutex, TX_WAIT_FOREVER);
    g_long_cmd.mode        = cmd->mode;
    g_long_cmd.target_kmh  = (float)cmd->target_speed_cms * 0.036f;
    g_long_cmd.headway     = cmd->headway;
    g_long_cmd.aeb_request = cmd->aeb_request;
    g_long_cmd.last_rx_ms  = HAL_GetTick();
    /* If Manager left CC/ACC, disengage immediately so the throttle path
     * below (MANUAL/LKA) can drive the motor on this same tick. */
    if (!wants_cc && g_cruise_control.state == CC_STATE_ACTIVE)
        CruiseControl_ForceOverride(&g_cruise_control);
    tx_mutex_put(&g_cc_mutex);

    /* --- 4. AEB request from Manager — absolute, like local AEB --------- */
    if (cmd->aeb_request)
    {
        tx_mutex_get(&g_cc_mutex, TX_WAIT_FOREVER);
        CruiseControl_ForceOverride(&g_cruise_control);
        tx_mutex_put(&g_cc_mutex);
        Motor_Stop();
        s_rx.actual_throttle_applied = 0;
        sys_log(ctx, "\033[1;31m[CTRL] AEB request from Manager\033[0m");
        return;
    }

    /* --- 5. Throttle by mode -------------------------------------------- */
    int8_t throttle;
    switch ((CtrlMode_t)cmd->mode) {
        case CTRL_MODE_MANUAL:
        case CTRL_MODE_LKA:
            throttle = cmd->throttle;
            break;
        case CTRL_MODE_CC:
        case CTRL_MODE_ACC:
            /* CC thread owns the motor while engaged — hand off. */
            return;
        case CTRL_MODE_DISABLED:
        default:
            Motor_Stop();
            s_rx.actual_throttle_applied = 0;
            return;
    }

    /* --- 5. Latched stops block forward (reverse always allowed) -------- */
    bool any_stop = (snap->emergency_stop_active || snap->aeb_stop_active);
    if (any_stop && throttle >= 0)
    {
        /* Throttle the log to ~1/s (CAN RX runs at ~100 Hz) to avoid flooding ITM */
        static uint32_t s_block_log_tick = 0;
        if (++s_block_log_tick >= 100u) {
            s_block_log_tick = 0;
            sys_log(ctx,
                "\033[1;33m[CTRL] BLOQUEADO: emerg=%d aeb_stop=%d aeb_state=%u"
                " aeb_lim=%u%% srf=%umm | Reverse OK\033[0m",
                (int)snap->emergency_stop_active,
                (int)snap->aeb_stop_active,
                (unsigned)snap->aeb_state,
                (unsigned)snap->aeb_speed_limit,
                (unsigned)snap->srf08_distance_mm);
        }
        Motor_Stop();
        s_rx.actual_throttle_applied = 0;
        return;
    }

    /* --- 6. Defer to CC loop if it's somehow active --------------------- */
    if (CruiseControl_IsActive(&g_cruise_control))
        return;

    if (throttle < -100) throttle = -100;
    if (throttle >  100) throttle =  100;

    /* --- 7. AEB/SRF08 throttle clamp (forward only) --------------------- */
    uint8_t limit = AEB_MAX_THROTTLE_PCT;
    if (snap->srf08_speed_limit < limit) limit = snap->srf08_speed_limit;
    if (snap->aeb_speed_limit   < limit) limit = snap->aeb_speed_limit;
    if (throttle > (int8_t)limit)
        throttle = (int8_t)limit;

    /* --- 8. Drive --------------------------------------------------------- */
    if (throttle == 0)
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

    s_rx.motor_current_estimate_ma = (uint16_t)(abs(throttle) * 20); /* ~2A @ 100% */
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

static void process_one_rx(SystemCtx* ctx, const CAN_Message_t* rx_msg, const VehicleState* snap)
{
  switch (rx_msg->id)
  {
    case CAN_ID_EMERGENCY_STOP: handle_emergency_stop(ctx, rx_msg, snap); break;
    case CAN_ID_CTRL_CMD:       handle_ctrl_cmd(ctx, rx_msg, snap);       break;
    case CAN_ID_HEARTBEAT_AGL:  handle_heartbeat_agl(ctx, rx_msg, snap);  break;
    case CAN_ID_CMD_RELAY:      handle_relay_cmd(ctx, rx_msg, snap);      break;
    case CAN_ID_CMD_INDICATOR:  handle_indicator_cmd(ctx, rx_msg);        break;

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

  sys_log(ctx, "\033[1;36m[CAN_RX] Aguardando comandos (0x001, 0x202 CtrlCmd, 0x700, 0x601, 0x602)...\033[0m");
}

void task_can_rx_step(SystemCtx* ctx)
{
  // periodic work
  send_motor_status_if_due();
  clear_mcp_flags_if_due(ctx);
  check_emergency_stop_timeout(ctx);   /* watchdog: auto-clear emergency se Manager perdeu o clear */

  /* Watchdog de staleness do 0x202 — Manager morto/CAN cortado com throttle
   * aplicado → Motor_Stop. Steering fica onde está. Só dispara se alguma vez
   * recebemos um comando (last_ctrl_cmd_tick != 0). */
  if (s_rx.last_ctrl_cmd_tick != 0 &&
      (HAL_GetTick() - s_rx.last_ctrl_cmd_tick) > CTRL_CMD_STALE_MS &&
      s_rx.actual_throttle_applied != 0)
  {
    Motor_Stop();
    s_rx.actual_throttle_applied = 0;
    sys_log(ctx, "\033[1;31m[CAN_RX] CtrlCmd timeout (>%ums sem 0x202) — Motor_Stop\033[0m",
            (unsigned)CTRL_CMD_STALE_MS);
  }

  //get the latest state snapshot for decision making
  VehicleState snap;

  tx_mutex_get(&ctx->state_mutex, TX_WAIT_FOREVER);
  snap = ctx->state;
  tx_mutex_put(&ctx->state_mutex);


  // Hard stop only for LATCHED (aeb_stop_active) or AGL emergency
  bool any_stop = (snap.emergency_stop_active || snap.aeb_stop_active);

  if (any_stop && s_rx.actual_throttle_applied > 0)
  {
    sys_log(ctx, "\033[1;33m[CAN_RX] STOP - Emergency/AEB LATCHED! (Reverse OK)\033[0m");
    Motor_Stop();
    s_rx.actual_throttle_applied = 0;
    tx_thread_sleep(CAN_RX_SLEEP_TICKS); /* must yield — same-priority tasks (SRF08, AEB) would starve */
    return;
  }

  // Proactive AEB speed enforcement: smoothly reduce motor output
  // between CAN commands to match the AEB kinematic speed limit.
  // This is what makes braking gradual instead of abrupt.
  //
  // Below ~15% PWM the motor can't spin but also doesn't brake
  // (it just coasts). Use Motor_Stop() (electrical brake) instead.
  #define AEB_BRAKE_THRESHOLD_PCT 20u  // = MIN_THROTTLE: below this motor doesn't spin

  if (s_rx.actual_throttle_applied > 0)
  {
    uint8_t limit = AEB_MAX_THROTTLE_PCT;
    if (snap.aeb_speed_limit < limit) limit = snap.aeb_speed_limit;

    if ((uint8_t)s_rx.actual_throttle_applied > limit)
    {
      if (limit < AEB_BRAKE_THRESHOLD_PCT)
      {
        // Below motor dead zone: use electrical brake, not low PWM coast
        Motor_Stop();
        s_rx.actual_throttle_applied = 0;
      }
      else
      {
        Motor_Forward(limit);
        s_rx.actual_throttle_applied = (int8_t)limit;
      }
    }
  }

  // RX drain with bounded work
  CAN_Message_t rx_msg;
  uint32_t processed = 0;

  while (processed < CAN_RX_MAX_FRAMES_PER_STEP)
  {
    /* Take SPI mutex for the check+read pair — prevents TX threads from
     * corrupting the SPI bus mid-transaction (race condition fix). */
    tx_mutex_get(&ctx->spi1_mutex, TX_WAIT_FOREVER);
    uint8_t has_msg = MCP2515_CheckReceive();
    uint8_t read_ok = 0;
    if (has_msg)
      read_ok = MCP2515_ReadMessage(&rx_msg);
    tx_mutex_put(&ctx->spi1_mutex);

    if (!has_msg)
      break;

    if (read_ok)
    {
      process_one_rx(ctx, &rx_msg, &snap);
      processed++;
    }
    else
    {
      break;
    }
  }

  // Small delay to avoid busy waiting (same)
  tx_thread_sleep(CAN_RX_SLEEP_TICKS);
}

/* Leituras single-byte dos valores realmente aplicados — usadas pelo
 * broadcast do CtrlStatus_t (0x213) em task_cruise_control.c. */
int8_t task_can_rx_actual_steering(void) { return s_rx.actual_steering_applied; }
int8_t task_can_rx_actual_throttle(void) { return s_rx.actual_throttle_applied; }
