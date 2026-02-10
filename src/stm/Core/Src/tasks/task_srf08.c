#include "./tasks/task_srf08.h"
#include "srf08.h"
#include "can_id.h"          // CAN_ID_*
#include "motor.h"            // Motor_Stop()
#include "crc.h"              // calculate_crc8()
#include "mcp2515.h"          // mcp_send_message()
#include "lcd1602.h"          // LCD1602_UpdateLine2()
#include "sys_log.h"          // sys_log(ctx,...)

// Keep these extern/global if other code depends on them exactly as globals
extern volatile uint8_t emergency_stop_active;
extern volatile uint8_t srf08_speed_limit;

// If these are currently file-static in app_threadx.c, move them to a module
// that owns SRF08 safety logic (this one), but keep semantics identical:
static EmergencyStopState_t srf08_emergency_state = ESTOP_STATE_NORMAL;

#ifndef SRF08_DISABLE_POLLING
#define SRF08_DISABLE_POLLING 1
#endif

// Preserve tick-based behavior exactly
enum {
  SRF08_MEAS_WAIT_TICKS   = 70,  // was tx_thread_sleep(70)
  SRF08_POLL_SLEEP_TICKS  = 5,   // was tx_thread_sleep(5)
  SRF08_MAX_POLLS         = 14   // 14 * 5 = 70 ticks
};

#define SRF08_FILTER_SIZE        5
#define SRF08_LIGHT_THRESHOLD    2

typedef struct
{
  SRF08_HandleTypeDef hsrf08;
  HAL_StatusTypeDef   init_status;
  uint32_t            can_send_counter;

  // logging throttles (same behavior)
  uint8_t err_log_counter;
  uint8_t timeout_log_counter;
  uint8_t srf08_log_counter;

  // filter state (same as static locals)
  uint16_t distance_buffer[SRF08_FILTER_SIZE];
  uint8_t  buffer_index;
  uint8_t  buffer_filled;
  uint16_t distance_mm_filtered;

  // one-time banner printed
  uint8_t printed_banner;
} TaskSRF08;

static TaskSRF08 s_srf;

void task_srf08_init(SystemCtx* ctx)
{
  (void)ctx;

  // reset task state deterministically
  TaskSRF08 z = {0};
  s_srf = z;

  sys_log(ctx, "\r\n[SRF08] Thread iniciada (PRIORIDADE ALTA - SAFETY CRITICAL)!\r\n");

  s_srf.init_status = SRF08_Init(&s_srf.hsrf08, &hi2c1, SRF08_DEFAULT_ADDR);

  if (s_srf.init_status == HAL_OK)
  {
    uint8_t version = SRF08_GetVersion(&s_srf.hsrf08);

    sys_log(ctx,
      "[SRF08] Sensor OK! Versao: %d | Emergency @ %u mm\r\n"
      "[SRF08] Configurado: Gain=12 (medio, melhor <200mm), Range=140 (~6m)\r\n"
      "[SRF08] Sleep fixo 70 ticks (sensor nao suporta polling)\r\n",
      version, SRF08_EMERGENCY_THRESHOLD_MM
    );
  }
  else
  {
    sys_log(ctx, "[SRF08] ERRO init! Status: %d\r\n", s_srf.init_status);
  }
}

static uint16_t srf08_apply_filter(uint16_t distance_mm_raw, uint8_t light)
{
  // Accept if (light >= threshold) OR (distance valid)
  // Reject only if light==0 AND distance==0 => keep previous filtered
  if (light >= SRF08_LIGHT_THRESHOLD || distance_mm_raw > 0)
  {
    s_srf.distance_buffer[s_srf.buffer_index] = distance_mm_raw;
    s_srf.buffer_index = (uint8_t)((s_srf.buffer_index + 1) % SRF08_FILTER_SIZE);

    if (!s_srf.buffer_filled && s_srf.buffer_index == 0)
      s_srf.buffer_filled = 1;

    uint32_t sum = 0;
    uint8_t count = s_srf.buffer_filled ? SRF08_FILTER_SIZE : s_srf.buffer_index;

    for (uint8_t i = 0; i < count; i++)
      sum += s_srf.distance_buffer[i];

    s_srf.distance_mm_filtered = (count > 0) ? (uint16_t)(sum / count) : 0;
  }

  return s_srf.distance_mm_filtered;
}

void task_srf08_step(SystemCtx* ctx)
{
  // 1) Start ranging
  HAL_StatusTypeDef ranging_status = SRF08_StartRanging(&s_srf.hsrf08);

  if (ranging_status != HAL_OK && ++s_srf.err_log_counter >= 15)
  {
    s_srf.err_log_counter = 0;
    sys_log(ctx,
      "\033[1;31m[SRF08] ERRO ao enviar comando ranging! Status: %d\033[0m\r\n",
      ranging_status
    );
  }

  // 2) Wait measurement complete (same behavior)
  uint8_t ready = 0;
  uint8_t poll_attempts = 0;

#if SRF08_DISABLE_POLLING
  tx_thread_sleep(SRF08_MEAS_WAIT_TICKS);
  ready = 1;
  poll_attempts = SRF08_MAX_POLLS;  // for logging equivalence
#else
  while (!ready && poll_attempts < SRF08_MAX_POLLS)
  {
    tx_thread_sleep(SRF08_POLL_SLEEP_TICKS);
    ready = SRF08_IsReady(&s_srf.hsrf08);
    poll_attempts++;
  }
#endif

  // 3) Timeout warning (same behavior)
  if (!ready && ++s_srf.timeout_log_counter >= 15)
  {
    s_srf.timeout_log_counter = 0;

    uint8_t cmd_reg = 0xFF;
    (void)HAL_I2C_Mem_Read(
      s_srf.hsrf08.hi2c, s_srf.hsrf08.addr,
      SRF08_REG_COMMAND, 1,
      &cmd_reg, 1, 100
    );

    sys_log(ctx,
      "\033[1;33m[SRF08] WARNING: Timeout! Polls=%u | CMD_REG=0x%02X (esperado 0x00)\033[0m\r\n",
      poll_attempts, cmd_reg
    );
  }

  // 4) Read distance + light (same as original: always read)
  uint16_t distance_cm = SRF08_GetDistanceCm(&s_srf.hsrf08);
  uint8_t  light       = SRF08_GetLight(&s_srf.hsrf08);

  uint16_t distance_mm_raw = (distance_cm == 0xFFFF) ? 0 : (uint16_t)(distance_cm * 10u);
  uint16_t distance_mm     = srf08_apply_filter(distance_mm_raw, light);

  // 5) Debug log every ~1s (same)
  if (++s_srf.srf08_log_counter >= 15)
  {
    s_srf.srf08_log_counter = 0;

    if (distance_cm == 0xFFFF)
    {
      sys_log(ctx, "\033[1;31m[SRF08] ERRO I2C ao ler distancia!\033[0m\r\n");
    }
    else
    {
      // preserve exact messages/formatting
      // note: sys_log should behave like printf+mutex
      sys_log(ctx, "\033[1;36m[SRF08] %u mm | L=%u | SpeedLimit=%u%%",
              distance_mm, light, srf08_speed_limit);

      if (srf08_speed_limit < 100)
        sys_log(ctx, " \033[1;33m[SLOWDOWN!]\033[1;36m");

      if (light == 0 && distance_cm == 0)
        sys_log(ctx, " <- SEM ECO");

      sys_log(ctx, "\033[0m\r\n");
    }
  }

  // 6) Speed control (same conditions)
  if (distance_mm >= SRF08_SLOWDOWN_THRESHOLD_MM || light == 0)
  {
    srf08_speed_limit = 100;
  }
  else if (distance_mm <= SRF08_EMERGENCY_THRESHOLD_MM && light > 0)
  {
    srf08_speed_limit = 0;
  }
  else if (light > 0)
  {
    srf08_speed_limit = SRF08_SLOWDOWN_SPEED_PERCENT;
  }

  // 7) Emergency stop logic (preserved)
  EmergencyStopState_t new_state = srf08_emergency_state;

  if (distance_mm < SRF08_EMERGENCY_THRESHOLD_MM && light > 0)
  {
    if (srf08_emergency_state != ESTOP_STATE_EMERGENCY)
    {
      Motor_Stop();

      new_state = ESTOP_STATE_EMERGENCY;
      emergency_stop_active = 1;

      EmergencyStop_t estop_frame;
      estop_frame.active = 1;
      estop_frame.source = 1;  // SRF08
      estop_frame.distance_mm = distance_mm;
      estop_frame.reason = 0x01;
      estop_frame.reserved[0] = 0;
      estop_frame.reserved[1] = 0;
      estop_frame.crc = calculate_crc8((uint8_t*)&estop_frame, sizeof(estop_frame) - 1);

      mcp_send_message(CAN_ID_EMERGENCY_STOP, (uint8_t*)&estop_frame, sizeof(estop_frame));

      sys_log(ctx,
        "\033[1;31m[SRF08 ESTOP!] %u mm < %u mm (L=%u)\033[0m\r\n",
        distance_mm, SRF08_EMERGENCY_THRESHOLD_MM, light
      );
    }
  }
  else if (srf08_emergency_state == ESTOP_STATE_EMERGENCY)
  {
    if (distance_mm >= SRF08_RECOVERY_THRESHOLD_MM || light == 0)
    {
      new_state = ESTOP_STATE_NORMAL;
      emergency_stop_active = 0;

      EmergencyStop_t estop_frame;
      estop_frame.active = 0;
      estop_frame.source = 1;
      estop_frame.distance_mm = distance_mm;
      estop_frame.reason = 0x00;
      estop_frame.reserved[0] = 0;
      estop_frame.reserved[1] = 0;
      estop_frame.crc = calculate_crc8((uint8_t*)&estop_frame, sizeof(estop_frame) - 1);

      mcp_send_message(CAN_ID_EMERGENCY_STOP, (uint8_t*)&estop_frame, sizeof(estop_frame));

      sys_log(ctx,
        "\033[1;32m[SRF08] RECOVERY! Dist=%u mm L=%u (CLEAR ENVIADO)\033[0m\r\n",
        distance_mm, light
      );
    }
  }
  else if (distance_mm >= SRF08_RECOVERY_THRESHOLD_MM && srf08_emergency_state == ESTOP_STATE_WARNING)
  {
    new_state = ESTOP_STATE_NORMAL;
  }
  else if (distance_mm < SRF08_RECOVERY_THRESHOLD_MM &&
           distance_mm >= SRF08_EMERGENCY_THRESHOLD_MM &&
           light > 0)
  {
    if (srf08_emergency_state == ESTOP_STATE_NORMAL)
      new_state = ESTOP_STATE_WARNING;
  }

  srf08_emergency_state = new_state;

  // 8) Periodic CAN send (same behavior)
  s_srf.can_send_counter++;

  if (light > 0 &&
      srf08_emergency_state != ESTOP_STATE_EMERGENCY &&
      (s_srf.can_send_counter % 1) == 0)
  {
    SRF08Distance_t srf08_frame;
    srf08_frame.distance_mm = distance_mm;
    srf08_frame.light_level = light;
    srf08_frame.gain = 0;
    srf08_frame.range_setting = 0;
    srf08_frame.reserved[0] = 0;
    srf08_frame.reserved[1] = 0;
    srf08_frame.status = 0x01;

    if (s_srf.init_status != HAL_OK)
      srf08_frame.status |= (1 << 1);

    mcp_send_message(CAN_ID_SRF08_DISTANCE, (uint8_t*)&srf08_frame, sizeof(srf08_frame));

    if ((s_srf.can_send_counter % 10) == 0)
    {
      sys_log(ctx, "[SRF08] %u mm | L=%u | State=%u\r\n",
              distance_mm, light, (unsigned)srf08_emergency_state);
    }
  }

  // 9) Keep "minimum lag" loop behavior
  tx_thread_relinquish();

  tx_mutex_get(&ctx->state_mutex, TX_WAIT_FOREVER);
  ctx->state.srf08_distance_mm = distance_mm;
  ctx->state.srf08_light = light;
  ctx->state.srf08_speed_limit_pct = srf08_speed_limit;
  tx_mutex_put(&ctx->state_mutex);
}
