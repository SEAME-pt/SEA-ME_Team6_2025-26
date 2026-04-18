#include "./tasks/task_srf08.h"
#include "srf08.h"
#include "can_id.h"          // CAN_ID_*
#include "motor_control.h"            // Motor_Stop()
#include "sys_helpers.h"              // calculate_crc8()
#include "mcp2515.h"          // mcp_send_message()


// Legacy globals kept for link compatibility but no longer written by SRF08.
// AEB (task_aeb.c) now owns all braking decisions.
extern volatile uint8_t emergency_stop_active;
extern volatile uint8_t srf08_speed_limit;

#ifndef SRF08_DISABLE_POLLING
#define SRF08_DISABLE_POLLING 1
#endif

// Preserve tick-based behavior exactly
enum {
  SRF08_MEAS_WAIT_TICKS   = 70,  // was tx_thread_sleep(70)
  SRF08_POLL_SLEEP_TICKS  = 5,   // was tx_thread_sleep(5)
  SRF08_MAX_POLLS         = 14   // 14 * 5 = 70 ticks
};

#define SRF08_FILTER_SIZE        3  // reduced from 5: less lag (~214ms vs ~357ms)
#define SRF08_LIGHT_THRESHOLD    2

/* Far-reading debounce: the sensor occasionally misses an echo and returns 0,
 * which maps to 6000 mm.  Only accept a reading above this threshold after
 * SRF08_FAR_DEBOUNCE_COUNT consecutive such readings.  A single close reading
 * immediately resets the debounce — safety-critical behaviour. */
#define SRF08_FAR_THRESHOLD_MM      2000U   /* anything above ~2 m is "possibly no echo" */
#define SRF08_FAR_DEBOUNCE_COUNT       3U   /* need 3 consecutive far readings (~210 ms) */

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

  // far-reading debounce state
  uint8_t  far_debounce_count;     /* consecutive far readings so far            */
  uint16_t last_near_distance_mm;  /* last confirmed close reading (safe fallback) */

  // one-time banner printed
  uint8_t printed_banner;
} TaskSRF08;

static TaskSRF08 s_srf;

void task_srf08_init(SystemCtx* ctx)
{
    // Zero-init the full struct to avoid stale filter data on reinit
    TaskSRF08 z = {0};
    s_srf = z;

    s_srf.hsrf08.hi2c = &hi2c1;
    s_srf.hsrf08.addr = SRF08_DEFAULT_ADDR;

    sys_log(ctx, "[SRF08] init...");

    // (A) check ready — use mutex to avoid I2C contention with IMU/ToF
    tx_mutex_get(&ctx->i2c1_mutex, TX_WAIT_FOREVER);
    HAL_StatusTypeDef st = HAL_I2C_IsDeviceReady(&hi2c1, SRF08_DEFAULT_ADDR, 3, 100);
    tx_mutex_put(&ctx->i2c1_mutex);

    if (st != HAL_OK) {
        s_srf.init_status = st;
        sys_log(ctx, "[SRF08] IsDeviceReady FAIL st=%d", st);
        return;
    }

    // sensor stabilizing delay (RTOS-friendly)
    tx_thread_sleep(100);

    // (B) set gain
    st = SRF08_SetGain(&s_srf.hsrf08, SRF08_RECOMMENDED_GAIN, ctx);

    if (st != HAL_OK) { s_srf.init_status = st; sys_log(ctx,"[SRF08] SetGain FAIL st=%d", st); return; }
    tx_thread_sleep(10);

    // (C) set range
    st = SRF08_SetRange(&s_srf.hsrf08, SRF08_RECOMMENDED_RANGE, ctx);

    s_srf.init_status = st;
    if (st != HAL_OK) { sys_log(ctx,"[SRF08] SetRange FAIL st=%d", st); return; }
    tx_thread_sleep(10);

    // (D) read version
    uint8_t ver = SRF08_GetVersion(&s_srf.hsrf08, ctx);

    // (E) verify gain register readback (only valid before first ranging)
    uint8_t gain_rb = 0xFF;
    tx_mutex_get(&ctx->i2c1_mutex, TX_WAIT_FOREVER);
    HAL_I2C_Mem_Read(&hi2c1, SRF08_DEFAULT_ADDR, SRF08_REG_GAIN, 1, &gain_rb, 1, 100);
    tx_mutex_put(&ctx->i2c1_mutex);

    sys_log(ctx, "[SRF08] OK version=%u | gain_reg=%u (expected %u) | range_reg=140",
            ver, gain_rb, SRF08_RECOMMENDED_GAIN);

    if (gain_rb != SRF08_RECOMMENDED_GAIN) {
        sys_log(ctx, "[SRF08] WARN: gain readback mismatch (got %u, expected %u) — a retentar...", gain_rb, SRF08_RECOMMENDED_GAIN);
        // Segunda tentativa
        HAL_StatusTypeDef st2 = SRF08_SetGain(&s_srf.hsrf08, SRF08_RECOMMENDED_GAIN, ctx);
        tx_thread_sleep(20);
        tx_mutex_get(&ctx->i2c1_mutex, TX_WAIT_FOREVER);
        HAL_I2C_Mem_Read(&hi2c1, SRF08_DEFAULT_ADDR, SRF08_REG_GAIN, 1, &gain_rb, 1, 100);
        tx_mutex_put(&ctx->i2c1_mutex);
        if (gain_rb != SRF08_RECOMMENDED_GAIN)
            sys_log(ctx, "[SRF08] ERRO: gain readback ainda errado após retry (st=%d, got=%u) — sensor pode estar mal configurado", st2, gain_rb);
        else
            sys_log(ctx, "[SRF08] gain corrigido na 2a tentativa OK");
    }
}

/*
void task_srf08_init(SystemCtx* ctx)
{
  (void)ctx;

  // reset task state deterministically
  TaskSRF08 z = {0};
  s_srf = z;

  sys_log(ctx, "[SRF08] Thread iniciada (PRIORIDADE ALTA - SAFETY CRITICAL)!");

  tx_mutex_get(&ctx->i2c1_mutex, TX_WAIT_FOREVER);
  s_srf.init_status = SRF08_Init(&s_srf.hsrf08, &hi2c1, SRF08_DEFAULT_ADDR);
  tx_mutex_put(&ctx->i2c1_mutex); 

  if (s_srf.init_status == HAL_OK)
  {
    uint8_t version = SRF08_GetVersion(&s_srf.hsrf08);

    sys_log(ctx,
      "[SRF08] Sensor OK! Versao: %d | Emergency @ %u mm"
      "[SRF08] Configurado: Gain=12 (medio, melhor <200mm), Range=140 (~6m)"
      "[SRF08] Sleep fixo 70 ticks (sensor nao suporta polling)",
      version, SRF08_EMERGENCY_THRESHOLD_MM
    );
  }
  else
  {
    sys_log(ctx, "[SRF08] ERRO init! Status: %d", s_srf.init_status);
  }
}*/

/* Insertion-sort median over the circular buffer (rejects spikes better than mean) */
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

    uint8_t count = s_srf.buffer_filled ? SRF08_FILTER_SIZE : s_srf.buffer_index;

    if (count == 0)
      return s_srf.distance_mm_filtered;

    // Copy buffer and sort to find median
    uint16_t tmp[SRF08_FILTER_SIZE];
    for (uint8_t i = 0; i < count; i++) tmp[i] = s_srf.distance_buffer[i];

    // Insertion sort (small buffer, acceptable cost)
    for (uint8_t i = 1; i < count; i++) {
      uint16_t key = tmp[i];
      int8_t j = (int8_t)i - 1;
      while (j >= 0 && tmp[j] > key) { tmp[j + 1] = tmp[j]; j--; }
      tmp[j + 1] = key;
    }

    s_srf.distance_mm_filtered = tmp[count / 2];
  }

  return s_srf.distance_mm_filtered;
}

void task_srf08_step(SystemCtx* ctx)
{
  // 1) Start ranging
  HAL_StatusTypeDef ranging_status = SRF08_StartRanging(&s_srf.hsrf08, ctx);

  if (ranging_status != HAL_OK)
  {
    if (++s_srf.err_log_counter >= 15)
    {
      s_srf.err_log_counter = 0;
      sys_log(ctx,
        "\033[1;31m[SRF08] ERRO ao enviar comando ranging! Status: %d\033[0m",
        ranging_status
      );
    }
    /* Ranging command failed — reading would be stale/invalid.
     * Sleep the normal measurement window so the task cadence stays the
     * same, then return without touching the filter or sending CAN.
     * This prevents 6000 mm / 0 lux from reaching the RPi5 datalogger. */
    tx_thread_sleep(SRF08_MEAS_WAIT_TICKS);
    return;
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
    ready = SRF08_IsReady(&s_srf.hsrf08, ctx);
    poll_attempts++;
  }
#endif

  // 3) Timeout warning
  if (!ready && ++s_srf.timeout_log_counter >= 15)
  {
    s_srf.timeout_log_counter = 0;

    uint8_t cmd_reg = 0xFF;
    tx_mutex_get(&ctx->i2c1_mutex, TX_WAIT_FOREVER);
    (void)HAL_I2C_Mem_Read(
      s_srf.hsrf08.hi2c, s_srf.hsrf08.addr,
      SRF08_REG_COMMAND, 1,
      &cmd_reg, 1, 100
    );
    tx_mutex_put(&ctx->i2c1_mutex);
    sys_log(ctx,
      "\033[1;33m[SRF08] WARNING: Timeout! Polls=%u | CMD_REG=0x%02X (esperado 0x00)\033[0m",
      poll_attempts, cmd_reg
    );
  }

  // 4) Read distance + light
  uint16_t distance_cm = SRF08_GetDistanceCm(&s_srf.hsrf08, ctx);
  uint8_t  light       = SRF08_GetLight(&s_srf.hsrf08, ctx);

  /* I2C read error — distance and light are both unreliable.
   * Do not update the filter or send CAN; the RPi5 will keep the last
   * valid reading rather than receiving 6000 mm / 0 lux garbage. */
  if (distance_cm == 0xFFFF)
  {
    if (++s_srf.srf08_log_counter >= 15)
    {
      s_srf.srf08_log_counter = 0;
      sys_log(ctx, "\033[1;31m[SRF08] ERRO I2C ao ler distancia!\033[0m");
    }
    tx_mutex_get(&ctx->state_mutex, TX_WAIT_FOREVER);
    ctx->state.srf08_valid = 0;
    tx_mutex_put(&ctx->state_mutex);
    return;
  }

  uint16_t distance_mm_raw;
  if (distance_cm == 0) {
    distance_mm_raw = (uint16_t)(SRF08_MAX_DISTANCE_CM * 10u);  // sem eco = sem obstáculo
  } else {
    distance_mm_raw = (uint16_t)(distance_cm * 10u);
  }

  // Eco do chão: leitura pequena não é obstáculo real → tratar como "livre"
  if (distance_mm_raw > 0 && distance_mm_raw < SRF08_MIN_VALID_DISTANCE_MM)
    distance_mm_raw = (uint16_t)(SRF08_MAX_DISTANCE_CM * 10u);

  uint16_t distance_mm = srf08_apply_filter(distance_mm_raw, light);

  /* Far-reading debounce: ignore isolated "no echo" spikes.
   * Only accept a far reading after SRF08_FAR_DEBOUNCE_COUNT consecutive
   * readings above SRF08_FAR_THRESHOLD_MM.  Any single close reading resets
   * the debounce immediately (safe-by-default: treat unknown as obstacle). */
  if (distance_mm > SRF08_FAR_THRESHOLD_MM)
  {
      if (s_srf.far_debounce_count < SRF08_FAR_DEBOUNCE_COUNT)
      {
          s_srf.far_debounce_count++;
          /* Not yet confirmed far — hold the last known close reading */
          distance_mm = s_srf.last_near_distance_mm;
      }
      /* else: debounce count reached → accept the far reading as-is */
  }
  else
  {
      /* Close reading → reset debounce and save as safe fallback */
      s_srf.far_debounce_count    = 0U;
      s_srf.last_near_distance_mm = distance_mm;
  }

  // 5) Debug log every ~1s
  if (++s_srf.srf08_log_counter >= 15)
  {
    s_srf.srf08_log_counter = 0;
    sys_log(ctx, "\033[1;36m[SRF08] raw=%u mm | filt=%u mm | L=%u\033[0m",
            distance_mm_raw, distance_mm, light);
  }

  // 6) Speed limit now handled entirely by AEB (task_aeb.c).
  //    SRF08 task only provides distance data.
  srf08_speed_limit = 100;  // no legacy limiting

  // 7) CAN send
  s_srf.can_send_counter++;

  {
    SRF08Distance_t srf08_frame;
    srf08_frame.distance_mm = distance_mm;
    srf08_frame.light_level = light;
    srf08_frame.gain = 0;
    srf08_frame.range_setting = 0;
    srf08_frame.reserved[0] = 0;
    srf08_frame.reserved[1] = 0;
    srf08_frame.status = 0x01;  /* ranging succeeded this cycle */

    if (s_srf.init_status != HAL_OK)
      srf08_frame.status |= (1 << 1);

    mcp_send_message(CAN_ID_SRF08_DISTANCE, (uint8_t*)&srf08_frame, sizeof(srf08_frame));

    if ((s_srf.can_send_counter % 10) == 0)
    {
      sys_log(ctx, "[SRF08] %u mm | L=%u | valid=1",
              distance_mm, light);
    }
  }

  tx_mutex_get(&ctx->state_mutex, TX_WAIT_FOREVER);
  ctx->state.srf08_distance_mm = distance_mm;
  ctx->state.srf08_light = light;
  ctx->state.srf08_speed_limit = srf08_speed_limit;
  //ctx->state.emergency_stop_active = emergency_stop_active;
  ctx->state.srf08_ts = tx_time_get();
  ctx->state.srf08_valid = 1;  /* always 1 here — error paths return early */
  tx_mutex_put(&ctx->state_mutex);

  // 9) Keep "minimum lag" loop behavior
  //tx_thread_relinquish();
}
