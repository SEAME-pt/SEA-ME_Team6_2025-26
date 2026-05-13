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

  // I2C bus recovery state
  /* i2c_err_count: consecutive cycles where StartRanging or GetDistanceCm
   *   returned HAL_ERROR / HAL_TIMEOUT (explicit bus fault).
   * dead_count: consecutive cycles where distance_cm==0 AND light==0
   *   simultaneously — the "silent lockup" signature.  The SRF08's I2C state
   *   machine can get stuck mid-transfer (no SDA stuck LOW, so the old
   *   bit-bang didn't help), causing ranging commands to be ignored.
   *   Light drops to 0 because the SRF08 only refreshes the light register
   *   when a ranging cycle completes.  In a lit environment, L≈245; L==0
   *   together with distance==0 for 8+ cycles is a reliable lockup indicator.
   *   Either counter reaching SRF08_RECOVERY_THRESHOLD triggers full recovery:
   *   peripheral reset + unconditional 9 SCL pulses + STOP + sensor re-init. */
  uint8_t  i2c_err_count;
  uint8_t  dead_count;        /* distance_cm==0 AND light==0 simultaneously     */
  uint8_t  recovery_count;    /* recoveries since last stable period            */
  uint8_t  good_cycles;       /* consecutive clean readings since last error    */

  // one-time banner printed
  uint8_t printed_banner;
} TaskSRF08;

/* After this many consecutive clean readings, reset recovery_count to 0.
 * This replenishes the recovery budget so the sensor can self-heal after
 * isolated I2C glitches during long-running sessions.
 * 100 cycles × 70 ms ≈ 7 s of confirmed stable operation. */
#define SRF08_STABLE_CYCLES_RESET  100u

/* Number of consecutive bad cycles before attempting I2C + sensor recovery.
 * At ~70 ms/cycle: 8 cycles ≈ 560 ms of confirmed failure before reset. */
#define SRF08_RECOVERY_THRESHOLD   8u
#define SRF08_MAX_RECOVERIES      10u   /* give up after this many attempts */

static TaskSRF08 s_srf;

void task_srf08_init(SystemCtx* ctx)
{
    // Zero-init the full struct — preserve recovery_count across re-inits
    // so we don't lose track of how many recoveries have been attempted.
    uint8_t saved_recovery_count = s_srf.recovery_count;
    TaskSRF08 z = {0};
    s_srf = z;
    s_srf.recovery_count = saved_recovery_count;
    /* Safe initial fallback: far-debounce outputs this until the first close
     * reading is confirmed.  Zero would be interpreted as "obstacle at sensor"
     * and trigger immediate emergency stop during the first 3 cycles. */
    s_srf.last_near_distance_mm = (uint16_t)(SRF08_MAX_DISTANCE_CM * 10u);

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

    // (B) set gain — retries one time on failure
    for (uint8_t attempt = 0; attempt < 2; attempt++) {
        st = SRF08_SetGain(&s_srf.hsrf08, SRF08_RECOMMENDED_GAIN, ctx);
        if (st == HAL_OK) break;
        tx_thread_sleep(20);
    }
    if (st != HAL_OK) { s_srf.init_status = st; sys_log(ctx,"[SRF08] SetGain FAIL st=%d", st); return; }
    tx_thread_sleep(10);

    // (C) set range — retries one time; range register cannot be read back so we
    //     fire-and-verify via a short ranging test after init.
    for (uint8_t attempt = 0; attempt < 2; attempt++) {
        st = SRF08_SetRange(&s_srf.hsrf08, SRF08_RECOMMENDED_RANGE, ctx);
        if (st == HAL_OK) break;
        tx_thread_sleep(20);
    }
    s_srf.init_status = st;
    if (st != HAL_OK) { sys_log(ctx,"[SRF08] SetRange FAIL st=%d", st); return; }
    tx_thread_sleep(10);

    // (D) read version
    uint8_t ver = SRF08_GetVersion(&s_srf.hsrf08, ctx);

    // (E) Verificação indirecta do range: faz um ranging de teste e confirma que
    //     o valor retornado é plausível (< max_range esperado).
    //     Se retornar 0 (sem eco), isso é OK — confirma que o comando chegou.
    //     Se retornar > max_range_mm, o range register pode não ter sido aceite.
    {
        const uint16_t expected_max_mm = (uint16_t)((SRF08_RECOMMENDED_RANGE + 1u) * 43u);
        (void)SRF08_StartRanging(&s_srf.hsrf08, ctx);
        tx_thread_sleep(75);  // aguarda medição
        uint16_t probe_cm  = SRF08_GetDistanceCm(&s_srf.hsrf08, ctx);
        uint8_t  probe_lux = SRF08_GetLight(&s_srf.hsrf08, ctx);
        uint16_t probe_mm  = (probe_cm == 0 || probe_cm == 0xFFFF)
                             ? 0u
                             : (uint16_t)(probe_cm * 10u);

        if (probe_cm == 0xFFFF) {
            sys_log(ctx, "[SRF08] WARN: leitura de verificação falhou (I2C err)");
        } else if (probe_mm > expected_max_mm) {
            sys_log(ctx,
                "[SRF08] WARN: leitura de verificação %u mm > max esperado %u mm"
                " — range register pode nao ter sido aceite!",
                probe_mm, expected_max_mm);
        } else {
            sys_log(ctx,
                "[SRF08] OK version=%u | gain=%u | range=%u (max~%umm) | probe=%umm L=%u",
                ver, SRF08_RECOMMENDED_GAIN, SRF08_RECOMMENDED_RANGE,
                expected_max_mm, probe_mm, probe_lux);
        }
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

/* ---------------------------------------------------------------------------
 * I2C bus + sensor recovery
 *
 * Called when HAL_I2C_Mem_Read returns an explicit error (HAL_ERROR / HAL_TIMEOUT)
 * on the post-write readback, or when StartRanging itself fails repeatedly.
 *
 * Strategy:
 *   1. DeInit the STM32 I2C1 peripheral — aborts any in-flight STM32-side state.
 *   2. Bit-bang 9 SCL pulses + STOP condition unconditionally on PB8/PB9.
 *      Even when SDA is HIGH, the sensor's I2C state machine may be stuck
 *      mid-transfer (waiting for a STOP that the DeInit ate).  The manual
 *      STOP completes that transaction and returns the sensor to idle.
 *   3. Restore GPIO AF, re-init I2C peripheral, wait 100ms for sensor to settle.
 *   4. Re-configure sensor gain/range via task_srf08_init().
 * ---------------------------------------------------------------------------*/
static void srf08_recover_i2c(SystemCtx* ctx)
{
    if (s_srf.recovery_count >= SRF08_MAX_RECOVERIES) {
        /* Too many attempts — sensor or bus is permanently broken.
         * Log once and stop trying. */
        if (s_srf.recovery_count == SRF08_MAX_RECOVERIES) {
            s_srf.recovery_count++;   /* prevent repeat log */
            sys_log(ctx,
                "\033[1;31m[SRF08] RECOVERY: limite atingido (%u). "
                "Sensor/barramento I2C1 inacessivel.\033[0m",
                SRF08_MAX_RECOVERIES);
        }
        return;
    }

    s_srf.recovery_count++;
    sys_log(ctx,
        "\033[1;33m[SRF08] RECOVERY #%u — a tentar recuperar barramento I2C1...\033[0m",
        s_srf.recovery_count);

    /* --- Step 1: reset the STM32 I2C peripheral --- */
    tx_mutex_get(&ctx->i2c1_mutex, TX_WAIT_FOREVER);

    HAL_I2C_DeInit(&hi2c1);
    tx_thread_sleep(5);

    /* --- Step 2: bit-bang 9 SCL pulses + STOP condition ---
     * Done UNCONDITIONALLY — even when SDA reads HIGH.
     *
     * Rationale: the sensor's I2C state machine can be stuck mid-transfer
     * (awaiting more data bytes or a STOP) even when SDA is not pulled LOW.
     * This happens when the STM32 I2C peripheral is DeInit'd while a
     * transaction is in progress: the sensor never sees the STOP condition.
     * 9 SCL clocks advance any stuck byte-transfer to completion;
     * the manual STOP condition then terminates it cleanly.
     *
     * Pins: PB8 = SCL (output OD), PB9 = SDA (input first, then output OD). */
    {
        GPIO_InitTypeDef gpio = {0};

        /* Read SDA before clocking — log whether it was stuck */
        gpio.Pin  = GPIO_PIN_9;
        gpio.Mode = GPIO_MODE_INPUT;
        gpio.Pull = GPIO_PULLUP;
        HAL_GPIO_Init(GPIOB, &gpio);
        uint8_t sda_was_low = (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9) == GPIO_PIN_RESET);

        /* SCL as open-drain output, start HIGH */
        gpio.Pin   = GPIO_PIN_8;
        gpio.Mode  = GPIO_MODE_OUTPUT_OD;
        gpio.Pull  = GPIO_NOPULL;
        gpio.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(GPIOB, &gpio);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
        tx_thread_sleep(1);

        /* 9 SCL pulses to flush any stuck byte in the sensor's I2C receiver */
        for (uint8_t i = 0; i < 9; i++) {
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
            tx_thread_sleep(1);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
            tx_thread_sleep(1);
        }

        /* Issue a STOP condition: SDA LOW → SDA HIGH while SCL is HIGH */
        gpio.Pin  = GPIO_PIN_9;
        gpio.Mode = GPIO_MODE_OUTPUT_OD;
        HAL_GPIO_Init(GPIOB, &gpio);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);  /* SDA LOW  */
        tx_thread_sleep(1);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);    /* SCL HIGH */
        tx_thread_sleep(1);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);    /* SDA HIGH → STOP */
        tx_thread_sleep(2);

        sys_log(ctx, "[SRF08] RECOVERY: 9 SCL + STOP enviados (SDA_antes=%s)",
                sda_was_low ? "LOW" : "HIGH");
    }

    /* Restore I2C alternate-function on PB8 + PB9 */
    {
        GPIO_InitTypeDef gpio = {0};
        gpio.Pin       = GPIO_PIN_8 | GPIO_PIN_9;
        gpio.Mode      = GPIO_MODE_AF_OD;
        gpio.Pull      = GPIO_NOPULL;
        gpio.Speed     = GPIO_SPEED_FREQ_LOW;
        gpio.Alternate = GPIO_AF4_I2C1;
        HAL_GPIO_Init(GPIOB, &gpio);
    }

    HAL_I2C_Init(&hi2c1);
    tx_thread_sleep(5);

    tx_mutex_put(&ctx->i2c1_mutex);

    /* Give the sensor time to stabilize before re-probing.
     * The bit-bang + STOP may have interrupted an ongoing ranging cycle;
     * the SRF08's 8051 needs a moment to recognise the bus is idle again. */
    tx_thread_sleep(100);

    /* --- Step 3: re-configure sensor --- */
    task_srf08_init(ctx);

    sys_log(ctx, "[SRF08] RECOVERY #%u concluido. init_status=%d",
            s_srf.recovery_count, s_srf.init_status);
}

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

/* Duration of the i2c1_mutex hold after the ranging write.
 *
 * SRF08 FW v11 holds SCL LOW after the STOP condition of the ranging
 * command, while its 8051 CPU processes the command and arms the transducer.
 * Any I2C1 transaction that starts during this window will: (a) see SCL LOW
 * and busy-wait in the HAL polling loop; (b) immediately issue a START when
 * SCL is released — right when the SRF08 state machine is transitional —
 * which corrupts the I2C stack ("silent lockup": HAL_OK, all bytes 0x00).
 *
 * 5 ms was confirmed insufficient. 20 ms still causes lockups in practice,
 * meaning the stretch lasts > 20 ms.  40 ms is the next step; if lockups
 * persist, raise toward 65 ms (the full measurement time). */
#define SRF08_SCL_STRETCH_GUARD_MS  65u

void task_srf08_step(SystemCtx* ctx)
{
  /* Pre-acquire I2C1 mutex BEFORE sending the ranging command.
   *
   * Without this, SRF08_StartRanging releases the mutex internally after the
   * write, and we re-acquire it a few instructions later — leaving a tiny but
   * real race window during which INA226 or motor tasks can grab the mutex.
   * If that happens, they see SCL still LOW (FW v11 stretch in progress),
   * busy-wait, then issue a START the moment SCL rises — corrupting the SRF08.
   *
   * ThreadX mutexes are recursive: SRF08_StartRanging's internal get/put
   * simply increments then decrements the ownership count (2→1), so the mutex
   * remains held (count ≥ 1) throughout.  No other task can acquire it. */
  tx_mutex_get(&ctx->i2c1_mutex, TX_WAIT_FOREVER);

  // 1) Start ranging (internal get→count 2, internal put→count 1; still held)
  HAL_StatusTypeDef ranging_status = SRF08_StartRanging(&s_srf.hsrf08, ctx);

  if (ranging_status != HAL_OK)
  {
    tx_mutex_put(&ctx->i2c1_mutex);   /* release before early-return */
    if (++s_srf.err_log_counter >= 15)
    {
      s_srf.err_log_counter = 0;
      sys_log(ctx,
        "\033[1;31m[SRF08] ERRO ao enviar comando ranging! Status: %d\033[0m",
        ranging_status
      );
    }
    /* Count consecutive I2C write failures for recovery trigger */
    s_srf.good_cycles = 0;
    if (++s_srf.i2c_err_count >= SRF08_RECOVERY_THRESHOLD)
    {
        s_srf.i2c_err_count = 0;
        s_srf.dead_count    = 0;
        srf08_recover_i2c(ctx);
    }
    tx_thread_sleep(SRF08_MEAS_WAIT_TICKS);
    return;
  }
  s_srf.i2c_err_count = 0;

  /* Hold through the SCL-stretch guard period, then release. */
  tx_thread_sleep(SRF08_SCL_STRETCH_GUARD_MS);
  tx_mutex_put(&ctx->i2c1_mutex);

  // 2) Wait remaining time for measurement to complete (~65 ms total needed,
  //    SRF08_SCL_STRETCH_GUARD_MS already elapsed above).
  uint8_t ready = 0;
  uint8_t poll_attempts = 0;

#if SRF08_DISABLE_POLLING
  tx_thread_sleep(SRF08_MEAS_WAIT_TICKS - SRF08_SCL_STRETCH_GUARD_MS);
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
    /* Explicit I2C read error — count toward recovery */
    s_srf.good_cycles = 0;
    if (++s_srf.i2c_err_count >= SRF08_RECOVERY_THRESHOLD)
    {
        s_srf.i2c_err_count = 0;
        s_srf.dead_count    = 0;
        srf08_recover_i2c(ctx);
    }
    tx_mutex_get(&ctx->state_mutex, TX_WAIT_FOREVER);
    ctx->state.srf08_valid = 0;
    tx_mutex_put(&ctx->state_mutex);
    return;
  }

  /* Silent lockup detection: SRF08 I2C state machine can get stuck mid-transfer
   * without any HAL error.  When stuck, ranging commands are silently ignored,
   * so the sensor returns stale data (0x00 in all registers).  The signature is
   * distance_cm==0 AND light==0 together for multiple cycles.
   *
   * Light==0 alone is legitimate (dark environment).
   * distance_cm==0 alone is legitimate (no echo within range).
   * Both simultaneously for many cycles in a lit environment = lockup.
   *
   * Recovery (9 SCL + STOP) resets the sensor's internal I2C state machine and
   * restores normal operation without a power cycle. */
  if (distance_cm == 0 && light == 0) {
    if (++s_srf.dead_count >= SRF08_RECOVERY_THRESHOLD) {
      /* Before recovering, confirm the sensor is actually locked up by reading
       * the version register (reg 0x00).  After a completed ranging cycle,
       * a healthy SRF08 returns its firmware version here (non-zero, e.g. 0x0B
       * for FW v11).  A locked-up sensor returns 0x00 for ALL register reads.
       *
       * This distinguishes a genuine I2C lockup from a legitimate physical
       * reading where both distance and light are zero (e.g. sensor facing a
       * surface within the dead zone in a dark environment).  Without this
       * check the recovery budget is exhausted in seconds whenever the car
       * is placed on a dark floor. */
      uint8_t ver = SRF08_GetVersion(&s_srf.hsrf08, ctx);

      if (ver != 0x00) {
        /* Sensor is alive — it just has nothing to report in this environment.
         * Reset dead_count so we don't keep probing every 8 cycles. */
        s_srf.dead_count = 0;
        sys_log(ctx,
          "[SRF08] %u ciclos dist=0 lux=0 mas sensor vivo (ver=0x%02X)"
          " — ambiente escuro/proximo, sem recuperacao",
          SRF08_RECOVERY_THRESHOLD, ver);
      } else {
        /* ver==0x00: sensor locked (all registers read back as 0x00). */
        s_srf.dead_count    = 0;
        s_srf.i2c_err_count = 0;
        sys_log(ctx,
          "\033[1;33m[SRF08] %u ciclos dist=0 lux=0 + ver=0x00 — I2C state machine"
          " bloqueada, a recuperar...\033[0m",
          SRF08_RECOVERY_THRESHOLD);
        srf08_recover_i2c(ctx);
        return;
      }
    }
  } else {
    s_srf.dead_count = 0;   /* any live reading resets the counter */

    /* Replenish recovery budget after sustained stable operation.
     * Prevents SRF08_MAX_RECOVERIES from being exhausted after many hours
     * of intermittent-but-recoverable I2C glitches. */
    if (++s_srf.good_cycles >= SRF08_STABLE_CYCLES_RESET) {
      s_srf.good_cycles    = 0;
      s_srf.recovery_count = 0;
    }
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
