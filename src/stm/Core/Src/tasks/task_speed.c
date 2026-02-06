// task_speed.c
#include "task_speed.h"

static uint32_t s_odometry_pulse_counter = 0;
static uint8_t  s_wheel_direction = 0; // 0=forward, 1=reverse

void task_speed_init(SystemCtx* ctx)
{
    sys_log(ctx, "\r\n[Speed] Thread iniciada!\r\n");

    /* Initialize speedometer once */
    Speedometer_Init();
}

void task_speed_step(SystemCtx* ctx)
{
    for (uint32_t i = 0; i < CAN_PERIOD_WHEEL_SPEED_MS; i++) {
        Speedometer_CountPulse();
        tx_thread_sleep(1);
    }

    Speedometer_CalculateSpeed();

    float speed_kmh = Speedometer_GetSpeed();
    float rpm_f     = Speedometer_GetRPM();

    if (speed_kmh < 0.0f)
        speed_kmh = 0.0f;

    /* Convert once: km/h → m/h */
    uint32_t speed_mh = (uint32_t)(speed_kmh * 1000.0f);

    /* Update shared VehicleState */
    tx_mutex_get(&ctx->state_mutex, TX_WAIT_FOREVER);
    ctx->state.rpm        = (int32_t)rpm_f;
    ctx->state.rpm_ts     = tx_time_get();
    ctx->state.speed_mh  = speed_mh;
    ctx->state.speed_ts   = tx_time_get();
    tx_mutex_put(&ctx->state_mutex);

    /* Build CAN frame (unchanged) */
    WheelSpeed_t wheel_frame;
    wheel_frame.rpm = (int16_t)rpm_f;

    if (speed_mh > 0) {
        s_odometry_pulse_counter++;
    }
    wheel_frame.total_pulses = s_odometry_pulse_counter;

    if (rpm_f > 0.5f)
        s_wheel_direction = 0;
    else if (rpm_f < -0.5f)
        s_wheel_direction = 1;

    wheel_frame.direction = s_wheel_direction;
    wheel_frame.status = 0;

    mcp_send_message(CAN_ID_WHEEL_SPEED,
                     (uint8_t*)&wheel_frame,
                     sizeof(wheel_frame));

    sys_log(ctx,
        "[Speed] RPM=%d | Speed=%lu m/h | Pulses=%lu\n",
        wheel_frame.rpm,
        (unsigned long)speed_mh,
        (unsigned long)wheel_frame.total_pulses);
}
