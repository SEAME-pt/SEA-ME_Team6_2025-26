#include "./tasks/task_tof.h"

/* ------------------------------------------------------------
 * Task-local state (owned by ToF task)
 * ------------------------------------------------------------ */
typedef struct {
    uint8_t status;
    uint8_t isAlive;
    uint8_t isReady;
    uint8_t initialized;

    VL53L5CX_ResultsData results;
} TofTaskCtx;

static TofTaskCtx s_tof;

static VL53L5CX_Configuration Dev;

/* Helper: reset sensor via XSHUT */
static void tof_hw_reset(SystemCtx* ctx)
{
    sys_log(ctx, "[ToF] Reset XSHUT...");

    HAL_GPIO_WritePin(Mems_VL53_xshut_GPIO_Port, Mems_VL53_xshut_Pin, GPIO_PIN_RESET);
    tx_thread_sleep(20);

    HAL_GPIO_WritePin(Mems_VL53_xshut_GPIO_Port, Mems_VL53_xshut_Pin, GPIO_PIN_SET);
    tx_thread_sleep(100);
}

void task_tof_init(SystemCtx* ctx)
{
    sys_log(ctx, "[ToF] Thread iniciada - 8x8 (Distance only)");

    /* Let other I2C users settle */
    tx_thread_sleep(500);

    /* 1) Configure I2C address */
    Dev.platform.address = 0x52;

    /* 2) Hardware reset */
    tof_hw_reset(ctx);

    /* 3) Check alive */
    s_tof.status = vl53l5cx_is_alive(&Dev, &s_tof.isAlive);
    if (!s_tof.isAlive || s_tof.status) {
        sys_log(ctx, "[ToF] ERRO: Sensor nao detetado! (alive=%u, status=%u)",
                s_tof.isAlive, s_tof.status);
        s_tof.initialized = 0;
        return;
    }

    /* 4) Init (FW upload) */
    s_tof.status = vl53l5cx_init(&Dev);
    if (s_tof.status) {
        sys_log(ctx, "[ToF] ERRO: Init falhou! status=%u", s_tof.status);
        s_tof.initialized = 0;
        return;
    }
    sys_log(ctx, "[ToF] VL53L5CX inicializado (8x8)!");

    /* 5) Set resolution 8x8 */
    s_tof.status = vl53l5cx_set_resolution(&Dev, VL53L5CX_RESOLUTION_8X8);
    if (s_tof.status) {
        sys_log(ctx, "[ToF] ERRO: Falha ao configurar 8x8! status=%u", s_tof.status);
        s_tof.initialized = 0;
        return;
    }

    /* 6) Set frequency 5Hz (same as original) */
    s_tof.status = vl53l5cx_set_ranging_frequency_hz(&Dev, 5);
    if (s_tof.status) {
        sys_log(ctx, "[ToF] AVISO: Falha ao configurar frequencia (status=%u)", s_tof.status);
        /* not fatal */
    }

    /* 7) Start ranging */
    s_tof.status = vl53l5cx_start_ranging(&Dev);
    if (s_tof.status) {
        sys_log(ctx, "[ToF] ERRO: Start ranging falhou! status=%u", s_tof.status);
        s_tof.initialized = 0;
        return;
    }

    sys_log(ctx, "[ToF] Ranging iniciado (8x8 @ 5Hz)!");
    s_tof.initialized = 1;
}

void task_tof_step(SystemCtx* ctx)
{
    if (!s_tof.initialized) {
        tx_thread_sleep(200);
        return;
    }

    s_tof.status = vl53l5cx_check_data_ready(&Dev, &s_tof.isReady);

    if (!s_tof.isReady) {
        tx_thread_sleep(200); /* ~5Hz pacing */
        return;
    }

    s_tof.status = vl53l5cx_get_ranging_data(&Dev, &s_tof.results);

    /* Find global minimum distance + nearest zone + count valid targets */
    uint16_t global_min_dist = 4000;
    uint8_t  global_nearest_zone = 255;
    uint8_t  valid_targets = 0;

    for (int i = 0; i < 64; i++) {
        uint8_t ts = s_tof.results.target_status[i];
        if (ts == 5 || ts == 9) {
            valid_targets++;
            uint16_t d = s_tof.results.distance_mm[i];
            if (d > 0 && d < global_min_dist) {
                global_min_dist = d;
                global_nearest_zone = (uint8_t)i;
            }
        }
    }

    /* Send ToF distance frame */
    ToFDistance_t tof_frame;
    tof_frame.min_distance_mm = global_min_dist;
    tof_frame.nearest_zone = global_nearest_zone;
    tof_frame.target_status = (global_nearest_zone != 255)
                                ? s_tof.results.target_status[global_nearest_zone]
                                : 0;
    tof_frame.detection_count = valid_targets;
    tof_frame.reserved[0] = 0;
    tof_frame.reserved[1] = 0;
    tof_frame.status = (s_tof.status != 0) ? (1 << 0) : 0;

    mcp_send_message(CAN_ID_TOF_DISTANCE, (uint8_t*)&tof_frame, sizeof(tof_frame));

    /* Update VehicleState (snapshot pattern) */
    tx_mutex_get(&ctx->state_mutex, TX_WAIT_FOREVER);
    ctx->state.tof_distance_mm = global_min_dist;
    ctx->state.tof_ts          = tx_time_get();
    tx_mutex_put(&ctx->state_mutex);

    /* Periodic log (~1s at 15Hz) */
    static uint8_t log_counter = 0;
    if (++log_counter >= 15) {
        log_counter = 0;
        sys_log(ctx, "[ToF] Dist=%u mm | Zone=%u | Targets=%u",
                global_min_dist, global_nearest_zone, valid_targets);
    }

    tx_thread_sleep(66);
}