#include "./tasks/task_environment.h"

/* ------------------------------------------------------------
 * Task-local context (private state of this task)
 * ------------------------------------------------------------ */
typedef struct {
    HAL_StatusTypeDef status_lps;
    HAL_StatusTypeDef status_hts;
    HAL_StatusTypeDef status_veml;

    float temperature;
    float pressure;
    float humidity;
    uint16_t ambient_light;
} EnvTaskCtx;

static EnvTaskCtx s_env;

/* ------------------------------------------------------------
 * Init
 * ------------------------------------------------------------ */
void task_environment_init(SystemCtx* ctx)
{
    sys_log(ctx, "[Environment] Thread iniciada!");

    /* I2C scan - debug only */
    #define ENABLE_I2C_SCAN 0
    #if ENABLE_I2C_SCAN
    sys_log(ctx, "[Environment] I2C2 scan...");
    I2C_Scan(&hi2c2, "I2C2");
    #endif

    /* Initialize sensors */
    s_env.status_lps  = LPS22HH_Init();
    s_env.status_hts  = HTS221_Init();
    s_env.status_veml = VEML6030_Init();

    /* Report only errors */
    if (s_env.status_lps != HAL_OK)
        sys_log(ctx, "[Environment] LPS22HH ERRO=%d", s_env.status_lps);
    if (s_env.status_hts != HAL_OK)
        sys_log(ctx, "[Environment] HTS221 ERRO=%d", s_env.status_hts);
    if (s_env.status_veml != HAL_OK)
        sys_log(ctx, "[Environment] VEML6030 ERRO=%d", s_env.status_veml);
}

/* ------------------------------------------------------------
 * One cycle (step)
 * ------------------------------------------------------------ */
void task_environment_step(SystemCtx* ctx)
{
    /* Read sensors */
    (void)LPS22HH_ReadTemperature(&s_env.temperature);
    (void)LPS22HH_ReadPressure(&s_env.pressure);
    (void)HTS221_ReadHumidity(&s_env.humidity);
    (void)VEML6030_ReadALS(&s_env.ambient_light);

    // if we opt for including these variables in the VehicleState snapshot
    // uncomment this and add the variables to the VehicleState struct in system_ctx.h
    /* Publish to shared VehicleState */
    /* tx_mutex_get(&ctx->state_mutex, TX_WAIT_FOREVER);
    ctx->state.env_temp_c = s_env.temperature;
    ctx->state.env_humidity = s_env.humidity;
    ctx->state.env_pressure_hpa = s_env.pressure;
    ctx->state.env_light_lux = (float)s_env.ambient_light;
    ctx->state.env_ts = tx_time_get();
    tx_mutex_put(&ctx->state_mutex); */

    /* Build Environment CAN frame (0x420) */
    Environment_t env_frame;

    env_frame.temperature = (int16_t)(s_env.temperature * 100.0f);   // 0.01°C
    env_frame.humidity    = (uint8_t)(s_env.humidity);               // 0..100%

    /* Ambient light: scale x100 (1234 lux -> 12), saturate to 255 */
    uint16_t light_scaled = s_env.ambient_light / 100;
    if (light_scaled > 255) light_scaled = 255;
    env_frame.ambient_light_x100 = (uint8_t)light_scaled;

    /* Pressure: if sensor gives hPa, Pa = hPa * 100 */
    uint32_t pressure_pa = (uint32_t)(s_env.pressure * 100.0f);
    env_frame.pressure = pressure_pa & 0xFFFFFF; /* 24 bits */

    /* status flags */
    env_frame.status = 0;
    if (s_env.status_lps  != HAL_OK) env_frame.status |= (1 << 0);
    if (s_env.status_hts  != HAL_OK) env_frame.status |= (1 << 1);
    if (s_env.status_veml != HAL_OK) env_frame.status |= (1 << 2);

    mcp_send_message(CAN_ID_ENVIRONMENT, (uint8_t*)&env_frame, sizeof(env_frame));

    HAL_GPIO_TogglePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin);

    sys_log(ctx,
        "[Environment] Enviado: %.2f°C | %.2f hPa | %.2f%% | %u lux",
        s_env.temperature, s_env.pressure, s_env.humidity, s_env.ambient_light
    );
}
