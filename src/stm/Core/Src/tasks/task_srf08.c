// ./tasks/task_srf08.c
#include "./tasks/task_srf08.h"

/* ---------------------------
 * Config
 * --------------------------- */
#ifndef SRF08_PERIOD_MS
#define SRF08_PERIOD_MS 70u /* ~14Hz, sensor does not like aggressive polling */
#endif

#define SRF08_FILTER_SIZE 5
#define SRF08_LIGHT_THRESHOLD 2

/* ---------------------------
 * Task-local state
 * --------------------------- */
typedef struct {
    SRF08_HandleTypeDef hsrf08;
    HAL_StatusTypeDef init_status;

    uint32_t can_send_counter;

    /* Moving average filter */
    uint16_t distance_buf[SRF08_FILTER_SIZE];
    uint8_t  buf_idx;
    uint8_t  buf_filled;
    uint16_t distance_mm_filtered;

    /* Debug counters */
    uint8_t err_log_counter;
    uint8_t timeout_log_counter;
    uint8_t srf08_log_counter;
} Srf08TaskCtx;

static Srf08TaskCtx s_srf;

/* If these exist elsewhere, keep extern. Prefer making them part of ctx later. */
volatile uint8_t srf08_speed_limit = 100; /* Speed limit 0-100% (100=full speed) */
volatile uint8_t emergency_stop_active = 0;
volatile EmergencyStopState_t srf08_emergency_state = ESTOP_STATE_NORMAL;

void task_srf08_init(SystemCtx* ctx)
{
    sys_log(ctx, "\r\n[SRF08] Thread iniciada (SAFETY CRITICAL)!\r\n");

    s_srf.can_send_counter = 0;
    s_srf.buf_idx = 0;
    s_srf.buf_filled = 0;
    s_srf.distance_mm_filtered = 0;
    s_srf.err_log_counter = 0;
    s_srf.timeout_log_counter = 0;
    s_srf.srf08_log_counter = 0;

    for (uint8_t i = 0; i < SRF08_FILTER_SIZE; i++)
        s_srf.distance_buf[i] = 0;

    s_srf.init_status = SRF08_Init(&s_srf.hsrf08, &hi2c1, SRF08_DEFAULT_ADDR);

    if (s_srf.init_status == HAL_OK) {
        uint8_t version = SRF08_GetVersion(&s_srf.hsrf08);
        sys_log(ctx,
            "[SRF08] Sensor OK! Versao: %u | Emergency @ %u mm\r\n"
            "[SRF08] Configurado: Gain=12, Range=140 | Period=%u ms\r\n",
            version, SRF08_EMERGENCY_THRESHOLD_MM, SRF08_PERIOD_MS);
    } else {
        sys_log(ctx, "[SRF08] ERRO init! Status: %d\r\n", s_srf.init_status);
    }
}

void task_srf08_step(SystemCtx* ctx)
{
    /* Start ranging */
    HAL_StatusTypeDef ranging_status = SRF08_StartRanging(&s_srf.hsrf08);
    if (ranging_status != HAL_OK && ++s_srf.err_log_counter >= 15) {
        s_srf.err_log_counter = 0;
        sys_log(ctx, "[SRF08] ERRO ranging cmd! Status=%d\r\n", ranging_status);
    }

    /* Fixed wait: SRF08 needs ~65ms */
    tx_thread_sleep(SRF08_PERIOD_MS);

    /* Read distance + light */
    uint16_t distance_cm = SRF08_GetDistanceCm(&s_srf.hsrf08);
    uint8_t  light       = SRF08_GetLight(&s_srf.hsrf08);

    /* Validity (IMPORTANT): don't use stale filtered values for control */
    uint8_t read_ok = (distance_cm != 0xFFFF);
    uint16_t distance_mm_raw = read_ok ? (uint16_t)(distance_cm * 10u) : 0u;
    uint8_t dist_valid = read_ok && (distance_mm_raw > 0u);

    /* Moving average filter: update only with valid distance */
    if (dist_valid) {
        s_srf.distance_buf[s_srf.buf_idx] = distance_mm_raw;
        s_srf.buf_idx = (uint8_t)((s_srf.buf_idx + 1u) % SRF08_FILTER_SIZE);
        if (!s_srf.buf_filled && s_srf.buf_idx == 0u) s_srf.buf_filled = 1u;

        uint32_t sum = 0;
        uint8_t count = s_srf.buf_filled ? SRF08_FILTER_SIZE : s_srf.buf_idx;
        for (uint8_t i = 0; i < count; i++) sum += s_srf.distance_buf[i];
        s_srf.distance_mm_filtered = (count > 0u) ? (uint16_t)(sum / count) : 0u;
    }
    /* else: keep previous filtered value, BUT do not use it for emergency decisions */

    uint16_t distance_mm = s_srf.distance_mm_filtered;

    /* Update VehicleState */
    tx_mutex_get(&ctx->state_mutex, TX_WAIT_FOREVER);
    ctx->state.srf08_distance_mm = distance_mm;       /* filtered value for telemetry */
    ctx->state.srf08_light       = light;
    ctx->state.srf08_valid       = dist_valid;        /* add this flag to VehicleState */
    ctx->state.srf08_ts          = tx_time_get();
    tx_mutex_put(&ctx->state_mutex);

    /* ---- SPEED LIMIT (avoid forcing full speed just because light==0) ---- */
    if (!dist_valid) {
        /* pick your policy:
           - 100 for permissive
           - SRF08_SLOWDOWN_SPEED_PERCENT for conservative */
        srf08_speed_limit = SRF08_SLOWDOWN_SPEED_PERCENT;
    } else if (distance_mm >= SRF08_SLOWDOWN_THRESHOLD_MM) {
        srf08_speed_limit = 100;
    } else if (distance_mm <= SRF08_EMERGENCY_THRESHOLD_MM) {
        srf08_speed_limit = 0;
    } else {
        srf08_speed_limit = SRF08_SLOWDOWN_SPEED_PERCENT;
    }

    /* ---- EMERGENCY STOP (use dist_valid; do NOT gate on light) ---- */
    EmergencyStopState_t new_state = srf08_emergency_state;

    if (dist_valid && distance_mm < SRF08_EMERGENCY_THRESHOLD_MM) {
        if (srf08_emergency_state != ESTOP_STATE_EMERGENCY) {
            Motor_Stop();

            new_state = ESTOP_STATE_EMERGENCY;
            emergency_stop_active = 1;

            EmergencyStop_t estop;
            estop.active = 1;
            estop.source = 1; /* SRF08 */
            estop.distance_mm = distance_mm;
            estop.reason = 0x01;
            estop.reserved[0] = 0;
            estop.reserved[1] = 0;
            estop.crc = calculate_crc8((uint8_t*)&estop, sizeof(estop) - 1);

            mcp_send_message(CAN_ID_EMERGENCY_STOP, (uint8_t*)&estop, sizeof(estop));

            sys_log(ctx, "[SRF08 ESTOP] %u mm < %u mm (L=%u)\r\n",
                    distance_mm, SRF08_EMERGENCY_THRESHOLD_MM, light);
        }
    } else if (srf08_emergency_state == ESTOP_STATE_EMERGENCY) {
        /* Recovery: only leave emergency when we have a valid safe distance */
        if (dist_valid && distance_mm >= SRF08_RECOVERY_THRESHOLD_MM) {
            new_state = ESTOP_STATE_NORMAL;
            emergency_stop_active = 0;

            EmergencyStop_t estop;
            estop.active = 0;
            estop.source = 1;
            estop.distance_mm = distance_mm;
            estop.reason = 0x00;
            estop.reserved[0] = 0;
            estop.reserved[1] = 0;
            estop.crc = calculate_crc8((uint8_t*)&estop, sizeof(estop) - 1);

            mcp_send_message(CAN_ID_EMERGENCY_STOP, (uint8_t*)&estop, sizeof(estop));

            sys_log(ctx, "[SRF08] RECOVERY Dist=%u mm (L=%u)\r\n", distance_mm, light);
        }
    } else if (dist_valid && distance_mm >= SRF08_RECOVERY_THRESHOLD_MM &&
               srf08_emergency_state == ESTOP_STATE_WARNING) {
        new_state = ESTOP_STATE_NORMAL;
    } else if (dist_valid &&
               distance_mm < SRF08_RECOVERY_THRESHOLD_MM &&
               distance_mm >= SRF08_EMERGENCY_THRESHOLD_MM) {
        if (srf08_emergency_state == ESTOP_STATE_NORMAL) {
            new_state = ESTOP_STATE_WARNING;
        }
    }

    srf08_emergency_state = new_state;

    /* ---- CAN publish (always publish, mark validity + errors) ---- */
    SRF08Distance_t srf_frame;
    srf_frame.distance_mm = distance_mm;
    srf_frame.light_level = light;
    srf_frame.gain = 0;
    srf_frame.range_setting = 0;
    srf_frame.reserved[0] = 0;
    srf_frame.reserved[1] = 0;

    srf_frame.status = 0;
    if (!dist_valid)           srf_frame.status |= (1 << 0); /* invalid distance */
    if (s_srf.init_status != HAL_OK) srf_frame.status |= (1 << 1); /* init error */
    if (!read_ok)              srf_frame.status |= (1 << 2); /* I2C read error */

    mcp_send_message(CAN_ID_SRF08_DISTANCE, (uint8_t*)&srf_frame, sizeof(srf_frame));

    /* periodic log (~1s) */
    if (++s_srf.srf08_log_counter >= 15) {
        s_srf.srf08_log_counter = 0;
        sys_log(ctx, "[SRF08] %u mm | L=%u | valid=%u | state=%u | limit=%u%%\r\n",
                distance_mm, light, (unsigned)dist_valid, (unsigned)new_state, (unsigned)srf08_speed_limit);
    }
}
