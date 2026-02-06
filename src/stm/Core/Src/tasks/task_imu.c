#include "./tasks/task_imu.h"

/* ------------------------------------------------------------
 * Task-local state (owned by IMU task)
 * ------------------------------------------------------------ */
typedef struct {
    ISM330DHCX_AxesRaw_t accel;
    ISM330DHCX_AxesRaw_t gyro;
    IIS2MDC_MagData_t    mag;

    HAL_StatusTypeDef init_ism;
    HAL_StatusTypeDef init_iis;

    HAL_StatusTypeDef st_accel;
    HAL_StatusTypeDef st_gyro;
    HAL_StatusTypeDef st_mag;
} ImuTaskCtx;

static ImuTaskCtx s_imu;

/* Helper: clamp int16 range */
static int16_t clamp_i16(int32_t v)
{
    if (v > 32767) return 32767;
    if (v < -32768) return -32768;
    return (int16_t)v;
}

void task_imu_init(SystemCtx* ctx)
{
    sys_log(ctx, "\r\n[IMU] Thread iniciada!\r\n");

    /* Init ISM330DHCX (accel+gyro) */
    s_imu.init_ism = ISM330DHCX_Init();
    if (s_imu.init_ism == HAL_OK)
        sys_log(ctx, "[IMU] ISM330DHCX inicializado com sucesso!\r\n");
    else
        sys_log(ctx, "[IMU] ERRO ao inicializar ISM330DHCX! Status: %d\r\n", s_imu.init_ism);

    /* Init IIS2MDC (mag) */
    s_imu.init_iis = IIS2MDC_Init();
    if (s_imu.init_iis == HAL_OK)
        sys_log(ctx, "[IMU] IIS2MDC (Magnetometro) inicializado com sucesso!\r\n");
    else
        sys_log(ctx, "[IMU] ERRO ao inicializar IIS2MDC! Status: %d\r\n", s_imu.init_iis);
}

void task_imu_step(SystemCtx* ctx)
{
    /* Read accel -> gyro -> mag (same sequencing as before) */
    HAL_StatusTypeDef st_accel = ISM330DHCX_ReadAccel(&s_imu.accel);
    tx_thread_sleep(5);

    HAL_StatusTypeDef st_gyro  = ISM330DHCX_ReadGyro(&s_imu.gyro);
    tx_thread_sleep(5);

    HAL_StatusTypeDef st_mag   = IIS2MDC_ReadMag(&s_imu.mag);

    /* Publish CAN frames independently */
    if (st_accel == HAL_OK) {
        ImuAccel_t f;
        f.acc_x = clamp_i16((int32_t)(s_imu.accel.x * 1000.0f));
        f.acc_y = clamp_i16((int32_t)(s_imu.accel.y * 1000.0f));
        f.acc_z = clamp_i16((int32_t)(s_imu.accel.z * 1000.0f));
        f.reserved = 0;
        f.status = (s_imu.init_ism != HAL_OK) ? (1 << 0) : 0;
        mcp_send_message(CAN_ID_IMU_ACCEL, (uint8_t*)&f, sizeof(f));
    }

    if (st_gyro == HAL_OK) {
        ImuGyro_t f;
        f.gyro_x = clamp_i16((int32_t)(s_imu.gyro.x * 10.0f));
        f.gyro_y = clamp_i16((int32_t)(s_imu.gyro.y * 10.0f));
        f.gyro_z = clamp_i16((int32_t)(s_imu.gyro.z * 10.0f));
        f.reserved = 0;
        f.status = (s_imu.init_ism != HAL_OK) ? (1 << 0) : 0;
        mcp_send_message(CAN_ID_IMU_GYRO, (uint8_t*)&f, sizeof(f));
    }

    if (st_mag == HAL_OK) {
        ImuMag_t f;
        f.mag_x = clamp_i16((int32_t)s_imu.mag.x); /* mG */
        f.mag_y = clamp_i16((int32_t)s_imu.mag.y);
        f.mag_z = clamp_i16((int32_t)s_imu.mag.z);
        f.reserved = 0;
        f.status = (s_imu.init_iis != HAL_OK) ? (1 << 0) : 0;
        mcp_send_message(CAN_ID_IMU_MAG, (uint8_t*)&f, sizeof(f));
    }

    /* update VehicleState only when we have a full coherent set */
    if (st_accel == HAL_OK && st_gyro == HAL_OK && st_mag == HAL_OK) {
        /* same scaling as CAN frames */
        int16_t ax_mg    = clamp_i16((int32_t)(s_imu.accel.x * 1000.0f));
        int16_t ay_mg    = clamp_i16((int32_t)(s_imu.accel.y * 1000.0f));
        int16_t az_mg    = clamp_i16((int32_t)(s_imu.accel.z * 1000.0f));

        int16_t gx_dps = clamp_i16((int32_t)(s_imu.gyro.x * 10.0f));
        int16_t gy_dps = clamp_i16((int32_t)(s_imu.gyro.y * 10.0f));
        int16_t gz_dps = clamp_i16((int32_t)(s_imu.gyro.z * 10.0f));

        int16_t mx_mG    = clamp_i16((int32_t)s_imu.mag.x);
        int16_t my_mG    = clamp_i16((int32_t)s_imu.mag.y);
        int16_t mz_mG    = clamp_i16((int32_t)s_imu.mag.z);

        uint32_t now = tx_time_get();

        tx_mutex_get(&ctx->state_mutex, TX_WAIT_FOREVER);

        ctx->state.accel_mg[0]      = ax_mg;
        ctx->state.accel_mg[1]      = ay_mg;
        ctx->state.accel_mg[2]      = az_mg;

        ctx->state.gyro_dps[0]    = gx_dps;
        ctx->state.gyro_dps[1]    = gy_dps;
        ctx->state.gyro_dps[2]    = gz_dps;

        ctx->state.mag_mG[0]        = mx_mG;
        ctx->state.mag_mG[1]        = my_mG;
        ctx->state.mag_mG[2]        = mz_mG;

        ctx->state.accel_ts         = now;
        ctx->state.gyro_ts          = now;
        ctx->state.mag_ts           = now;
        ctx->state.imu_ts           = now;

        tx_mutex_put(&ctx->state_mutex);
    }

    /* log only when we have a full set */
    if (st_accel == HAL_OK && st_gyro == HAL_OK && st_mag == HAL_OK) {
        sys_log(ctx,
            "[IMU] Accel(%.2f, %.2f, %.2f)g | Gyro(%.1f, %.1f, %.1f)dps | Mag(%.0f, %.0f, %.0f)mG\r\n",
            s_imu.accel.x, s_imu.accel.y, s_imu.accel.z,
            s_imu.gyro.x,  s_imu.gyro.y,  s_imu.gyro.z,
            s_imu.mag.x,   s_imu.mag.y,   s_imu.mag.z
        );
    }

    uint32_t imu_sleep = (CAN_PERIOD_IMU_FAST_MS > 10) ? (CAN_PERIOD_IMU_FAST_MS - 10) : 0;
    tx_thread_sleep(imu_sleep);
}
