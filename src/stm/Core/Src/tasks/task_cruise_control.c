/*
 * task_cruise_control.c
 *
 * Cruise Control + ACC — ThreadX task @ 20 Hz.
 * Engagement is driven by the latest CtrlCmd_t (0x202) received via task_can_rx:
 *   - mode = CC  → speed PI tracks g_long_cmd.target_kmh
 *   - mode = ACC → speed PI tracks min(target_kmh, v_safe_from_gap)
 *   - any other mode (or stale cmd) → CC disengaged
 * Broadcasts CC status on 0x213 at 10 Hz.
 */

#include "tasks/task_cruise_control.h"
#include "can_id.h"
#include "can_tx.h"
#include "motor_control.h"
#include "mcp2515.h"
#include "sys_helpers.h"
#include "stm32u5xx_hal.h"  /* HAL_GetTick */

/* Tunables specific to the ACC gap clamp */
#define ACC_CMD_TIMEOUT_MS      200U   /* CC disengages if no 0x202 in this window */
#define ACC_GAP_MIN_VALID_CM    5      /* below = sensor noise */
#define ACC_GAP_MAX_VALID_CM    400    /* above = no lead vehicle in range */
#define ACC_GAP_HARD_STOP_CM    20     /* under this → target=0 (full stop) */

CruiseControl_t   g_cruise_control;
LongitudinalCmd_t g_long_cmd;
TX_MUTEX          g_cc_mutex;

static float headway_seconds(uint8_t setting)
{
    switch (setting) {
        case HEADWAY_CLOSE: return 0.8f;
        case HEADWAY_FAR:   return 2.2f;
        case HEADWAY_MEDIUM:
        default:            return 1.5f;
    }
}

/* Activate the CC state machine without going through ProcessCommand.
 * Caller must hold g_cc_mutex. Mirrors the CC_CMD_ACTIVATE branch. */
static void cc_engage(float current_kmh)
{
    if (g_cruise_control.state == CC_STATE_ACTIVE) return;
    g_cruise_control.state               = CC_STATE_ACTIVE;
    g_cruise_control.integral            = 0.0f;
    g_cruise_control.last_error          = 0.0f;
    g_cruise_control.stall_timer_ms      = 0;
    g_cruise_control.activation_grace_ms = 0;
    g_cruise_control.active_target_kmh   = current_kmh;
}

void task_cc_init(SystemCtx *ctx)
{
    CruiseControl_Init(&g_cruise_control);
    g_long_cmd.mode       = CTRL_MODE_DISABLED;
    g_long_cmd.target_kmh = 0.0f;
    g_long_cmd.headway    = HEADWAY_MEDIUM;
    g_long_cmd.aeb_request = 0;
    g_long_cmd.last_rx_ms = 0;
    tx_mutex_create(&g_cc_mutex, "CC Mutex", TX_NO_INHERIT);

    sys_log(ctx, "[CC] CC/ACC initialized (PI Kp=%.1f Ki=%.1f)", CC_KP, CC_KI);
}

void task_cc_step(SystemCtx *ctx)
{
    /* --- Snapshot vehicle state --- */
    VehicleState snap;
    tx_mutex_get(&ctx->state_mutex, TX_WAIT_FOREVER);
    snap = ctx->state;
    tx_mutex_put(&ctx->state_mutex);

    float current_kmh = (float)snap.speed_mh / 1000.0f;

    tx_mutex_get(&g_cc_mutex, TX_WAIT_FOREVER);

    /* --- Resolve engagement decision from the latest CtrlCmd_t --- */
    uint32_t now_ms = HAL_GetTick();
    bool     stale  = (now_ms - g_long_cmd.last_rx_ms) > ACC_CMD_TIMEOUT_MS;
    uint8_t  mode   = stale ? CTRL_MODE_DISABLED : g_long_cmd.mode;

    bool wants_cc  = (mode == CTRL_MODE_CC || mode == CTRL_MODE_ACC);

    /* Absolute overrides — kill CC regardless of mode */
    if (snap.aeb_stop_active || snap.emergency_stop_active || g_long_cmd.aeb_request)
    {
        CruiseControl_ForceOverride(&g_cruise_control);
    }
    else if (!wants_cc)
    {
        /* Manager wants MANUAL/LKA/DISABLED — bow out so handle_ctrl_cmd drives. */
        if (CruiseControl_IsActive(&g_cruise_control))
            CruiseControl_ForceOverride(&g_cruise_control);
    }
    else
    {
        /* Wants CC or ACC. Compute the effective target the PI should chase. */
        float effective_kmh = g_long_cmd.target_kmh;

        if (mode == CTRL_MODE_ACC)
        {
            /* SRF08 gap clamp — "most restrictive wins". */
            uint16_t gap_cm = snap.srf08_distance_mm / 10;
            bool gap_valid = snap.srf08_valid &&
                             gap_cm >= ACC_GAP_MIN_VALID_CM &&
                             gap_cm <= ACC_GAP_MAX_VALID_CM;
            if (gap_valid)
            {
                if (gap_cm < ACC_GAP_HARD_STOP_CM)
                {
                    effective_kmh = 0.0f;
                }
                else
                {
                    float hw_s     = headway_seconds(g_long_cmd.headway);
                    float gap_m    = (float)gap_cm * 0.01f;
                    float v_safe   = (gap_m / hw_s) * 3.6f;  /* km/h */
                    if (v_safe < effective_kmh) effective_kmh = v_safe;
                }
            }
            /* else: no lead detected — use raw target unchanged */
        }

        /* Push setpoint into CC; rate limiter inside CC smooths the step. */
        g_cruise_control.target_speed_kmh = effective_kmh;

        if (g_cruise_control.state != CC_STATE_ACTIVE)
            cc_engage(current_kmh);
    }

    CruiseControl_Update(&g_cruise_control, current_kmh);

    bool    cc_active = CruiseControl_IsActive(&g_cruise_control);
    int8_t  throttle  = 0;
    if (cc_active)
    {
        float t = CruiseControl_GetThrottle(&g_cruise_control);
        throttle = (t < 0.0f) ? 0 : (t > 100.0f) ? 100 : (int8_t)t;
        if (throttle > (int8_t)snap.aeb_speed_limit)
            throttle = (int8_t)snap.aeb_speed_limit;
    }

    /* Capture status fields under mutex for the 0x213 broadcast */
    uint8_t  st_state    = (uint8_t)g_cruise_control.state;
    uint16_t st_target   = (uint16_t)(g_cruise_control.target_speed_kmh * 100.0f);
    uint16_t st_current  = (uint16_t)(g_cruise_control.current_speed_kmh * 100.0f);
    int8_t   st_throttle = (int8_t)g_cruise_control.applied_throttle;
    uint8_t  st_counter  = g_cruise_control.tx_counter++;
    if (g_cruise_control.tx_counter > 14) g_cruise_control.tx_counter = 0;
    uint8_t  st_mode     = mode;

    tx_mutex_put(&g_cc_mutex);

    /* --- Apply throttle (outside mutex) ---
     * Edge case: when CC disengages (AEB latch, mode change, stale cmd) we
     * may have been driving the motor; stop it once on the falling edge so
     * we don't keep coasting at the last commanded throttle. */
    static bool was_cc_active = false;
    if (cc_active)
    {
        if (throttle > 0)
            Motor_Forward((uint8_t)throttle);
        else
            Motor_Stop();
    }
    else if (was_cc_active)
    {
        Motor_Stop();
    }
    was_cc_active = cc_active;

    /* --- Broadcast CC status to AGL at 10 Hz (legacy 0x213 layout) --- */
    static uint32_t last_status_ms = 0;
    uint32_t now_tick = (uint32_t)tx_time_get();
    if ((now_tick - last_status_ms) >= CC_STATUS_PERIOD_MS)
    {
        last_status_ms = now_tick;

        CruiseControlStatus_t status;
        status.state            = st_state;
        status.target_speed     = st_target;
        status.current_speed    = st_current;
        status.applied_throttle = st_throttle;
        status.counter          = st_counter;
        status.crc              = calculate_crc8((uint8_t *)&status, sizeof(status) - 1);

        tx_mutex_get(&ctx->spi1_mutex, TX_WAIT_FOREVER);
        mcp_send_message(CAN_ID_CC_STATUS, (uint8_t *)&status, sizeof(status));

        sys_log(ctx, "[CC] mode=%u state=%u tgt=%u cur=%u thr=%d",
                st_mode, st_state, st_target, st_current, st_throttle);

        tx_mutex_put(&ctx->spi1_mutex);
    }
}
