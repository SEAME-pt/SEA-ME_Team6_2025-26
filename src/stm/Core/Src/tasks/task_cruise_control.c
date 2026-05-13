/*
 * task_cruise_control.c
 *
 * Cruise Control V1 — ThreadX task
 * Runs PI loop at 20 Hz, drives motor directly when active,
 * and broadcasts CC status on CAN at 10 Hz.
 *
 * Created on: Mar 2026
 *     Author: rcosta-c
 */

#include "tasks/task_cruise_control.h"
#include "can_id.h"
#include "can_tx.h"
#include "motor_control.h"
#include "mcp2515.h"
#include "sys_helpers.h"

/* Single global CC instance — shared read-only with task_can_rx */
CruiseControl_t g_cruise_control;

/* Protects g_cruise_control against concurrent access from CAN RX thread */
TX_MUTEX g_cc_mutex;


void task_cc_init(SystemCtx *ctx)
{
    CruiseControl_Init(&g_cruise_control);
    tx_mutex_create(&g_cc_mutex, "CC Mutex", TX_NO_INHERIT);



    sys_log(ctx, "[CC] Cruise Control V1 inicializado (PI Kp=%.1f Ki=%.1f)", CC_KP, CC_KI);
}

void task_cc_step(SystemCtx *ctx)
{
    /* --- Snapshot shared state --- */
    VehicleState snap;
    tx_mutex_get(&ctx->state_mutex, TX_WAIT_FOREVER);
    snap = ctx->state;
    tx_mutex_put(&ctx->state_mutex);

    float current_kmh = (float)snap.speed_mh / 1000.0f;

    /* --- Protect g_cruise_control: update + capture status fields atomically --- */
    tx_mutex_get(&g_cc_mutex, TX_WAIT_FOREVER);

    if (snap.aeb_stop_active || snap.emergency_stop_active)
        CruiseControl_ForceOverride(&g_cruise_control);

    CruiseControl_Update(&g_cruise_control, current_kmh);

    bool    cc_active = CruiseControl_IsActive(&g_cruise_control);
    int8_t  throttle  = 0;
    if (cc_active)
    {
        float t = CruiseControl_GetThrottle(&g_cruise_control);
        /* CC is forward-only: clamp to [0, 100] */
        throttle = (t < 0.0f) ? 0 : (t > 100.0f) ? 100 : (int8_t)t;
        if (throttle > (int8_t)snap.aeb_speed_limit)
            throttle = (int8_t)snap.aeb_speed_limit;
    }

    /* Capture status fields while still holding the mutex */
    uint8_t  st_state    = (uint8_t)g_cruise_control.state;
    uint16_t st_target   = (uint16_t)(g_cruise_control.target_speed_kmh * 100.0f);
    uint16_t st_current  = (uint16_t)(g_cruise_control.current_speed_kmh * 100.0f);
    int8_t   st_throttle = (int8_t)g_cruise_control.applied_throttle;
    uint8_t  st_counter  = g_cruise_control.tx_counter++;
    if (g_cruise_control.tx_counter > 14) g_cruise_control.tx_counter = 0;

    tx_mutex_put(&g_cc_mutex);

    /* --- Apply throttle to motor (hardware call — outside mutex) --- */
    /* CC is forward-only: throttle is always >= 0 here */
    if (cc_active)
    {
        if (throttle > 0)
            Motor_Forward((uint8_t)throttle);
        else
            Motor_Stop();
    }

    /* --- Broadcast CC status to AGL at 10 Hz --- */
    static uint32_t last_status_ms = 0;
    uint32_t now = (uint32_t)tx_time_get();
    if ((now - last_status_ms) >= CC_STATUS_PERIOD_MS)
    {
        last_status_ms = now;

        CruiseControlStatus_t status;
        status.state            = st_state;
        status.target_speed     = st_target;
        status.current_speed    = st_current;
        status.applied_throttle = st_throttle;
        status.counter          = st_counter;
        status.crc              = calculate_crc8((uint8_t *)&status, sizeof(status) - 1);

        tx_mutex_get(&ctx->spi1_mutex, TX_WAIT_FOREVER);
        mcp_send_message(CAN_ID_CC_STATUS, (uint8_t *)&status, sizeof(status));


        sys_log(ctx, "[CC] State=%d | Target Speed=%d | Current Speed=%d |/h\n",
                  status.state, status.target_speed, status.current_speed );

        tx_mutex_put(&ctx->spi1_mutex);
    }
}
