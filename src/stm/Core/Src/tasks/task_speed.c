// task_speed.c
#include "./tasks/task_speed.h"

static uint32_t s_odometry_pulse_counter = 0;
static uint8_t  s_wheel_direction = 0; // 0=forward, 1=reverse

void task_speed_init(SystemCtx* ctx)
{
    sys_log(ctx, "[Speed] Thread iniciada!");

    /* Initialize speedometer once */
    Speedometer_Init(ctx);
}

void task_speed_step(SystemCtx* ctx)
{
    /* Let EXTI ISR count pulses; just wait a fixed window */
    tx_thread_sleep(CAN_PERIOD_WHEEL_SPEED_MS);

    /* Convert counted pulses into RPM/speed */
    Speedometer_CalculateSpeed(ctx);

    //sys_log(ctx, "[Speedometer] pulses=%u", pulse_hits);

    float speed_kmh = Speedometer_GetSpeed();
    float rpm_f     = Speedometer_GetRPM();

    /* Low-pass filters */
    static float speed_kmh_filtered = 0.0f;
    static float rpm_filtered = 0.0f;
    const float alpha = 0.2f;

    speed_kmh_filtered += alpha * (speed_kmh - speed_kmh_filtered);
    rpm_filtered       += alpha * (rpm_f     - rpm_filtered);

    if (speed_kmh_filtered < 0.0f)
        speed_kmh_filtered = 0.0f;

    uint32_t speed_mh = (uint32_t)(speed_kmh_filtered * 1000.0f);

    tx_mutex_get(&ctx->state_mutex, TX_WAIT_FOREVER);
    ctx->state.rpm      = (int32_t)rpm_filtered;
    ctx->state.rpm_ts   = tx_time_get();
    ctx->state.speed_mh = speed_mh;
    ctx->state.speed_ts = tx_time_get();
    tx_mutex_put(&ctx->state_mutex);

    WheelSpeed_t wheel_frame;
    wheel_frame.rpm = (int16_t)rpm_filtered;
    if (speed_mh > 0) {
        s_odometry_pulse_counter++;
    }
    wheel_frame.total_pulses = s_odometry_pulse_counter;

    if (rpm_filtered > 0.5f)
        s_wheel_direction = 0;
    else if (rpm_filtered < -0.5f)
        s_wheel_direction = 1;

    wheel_frame.direction = s_wheel_direction;
    wheel_frame.status = 0;

    mcp_send_message(CAN_ID_WHEEL_SPEED,
                     (uint8_t*)&wheel_frame,
                     sizeof(wheel_frame));

    //sys_log(ctx,
    //    "[Speed] RPM=%d | Speed=%lu m/h | Pulses=%lu",
    //    wheel_frame.rpm,
    //    (unsigned long)speed_mh,
    //    (unsigned long)wheel_frame.total_pulses);
}
