// ./tasks/task_battery.c
#include "./tasks/task_battery.h"

/* LiPo 3S voltage-based SOC estimate */
#define BATTERY_FULL_MV   12600u   /* 4.2V * 3 */
#define BATTERY_EMPTY_MV   9900u   /* 3.3V * 3 */

typedef struct {
    HAL_StatusTypeDef ina_status;
    INA226_Data_t     ina;
    uint8_t           cycles;

    /* last computed state */
    uint16_t voltage_mv;
    int16_t  current_ma;
    uint8_t  soc;
    int8_t   temperature;
    uint8_t  status;
} BatteryTaskCtx;

static BatteryTaskCtx s_batt;

static uint8_t soc_from_voltage_mv(uint16_t mv)
{
    if (mv >= BATTERY_FULL_MV)  return 100;
    if (mv <= BATTERY_EMPTY_MV) return 0;

    return (uint8_t)(((uint32_t)(mv - BATTERY_EMPTY_MV) * 100u) /
                     (uint32_t)(BATTERY_FULL_MV - BATTERY_EMPTY_MV));
}

static int8_t temp_estimate_from_current_ma(int16_t ma)
{
    int32_t a = abs((int)ma);
    if (a > 5000) return 35;
    if (a > 2000) return 30;
    return 25;
}

void task_battery_init(SystemCtx* ctx)
{
    sys_log(ctx, "\r\n[Battery] Thread iniciada!\r\n");

    s_batt.voltage_mv = 0;
    s_batt.current_ma = 0;
    s_batt.soc = 100;
    s_batt.temperature = 25;
    s_batt.cycles = 0;
    s_batt.status = 0;

    s_batt.ina_status = INA226_Init(&hi2c1);

    if (s_batt.ina_status == HAL_OK) {
        uint16_t mfg_id = INA226_GetManufacturerID();
        uint16_t die_id = INA226_GetDieID();
        sys_log(ctx, "[Battery] INA226 inicializado! MFG=0x%04X DIE=0x%04X\r\n", mfg_id, die_id);
    } else {
        sys_log(ctx, "[Battery] ERRO ao inicializar INA226! Status: %d\r\n", s_batt.ina_status);
    }
}

void task_battery_step(SystemCtx* ctx)
{
    /* Default: keep last values, update status */
    uint8_t battery_status = 0;

    if (s_batt.ina_status == HAL_OK) {
        HAL_StatusTypeDef rd = INA226_ReadAll(&s_batt.ina);

        if (rd == HAL_OK && s_batt.ina.valid) {
            s_batt.voltage_mv = (uint16_t)(s_batt.ina.voltage_V * 1000.0f);
            s_batt.current_ma = (int16_t)(s_batt.ina.current_A * 1000.0f);

            s_batt.soc = soc_from_voltage_mv(s_batt.voltage_mv);
            s_batt.temperature = temp_estimate_from_current_ma(s_batt.current_ma);

            battery_status = 0;
            if (s_batt.voltage_mv < 10500u)        battery_status |= (1 << 0);  /* warning */
            if (s_batt.voltage_mv < BATTERY_EMPTY_MV) battery_status |= ERROR_FLAG_UNDERVOLTAGE;
            if (s_batt.voltage_mv > 13000u)        battery_status |= ERROR_FLAG_OVERVOLTAGE;
            if (s_batt.soc < 20u)                  battery_status |= (1 << 2);  /* low batt */
            if (abs((int)s_batt.current_ma) > 15000) battery_status |= (1 << 4); /* overcurrent */
        } else {
            battery_status = ERROR_FLAG_SENSOR_FAULT;
        }
    } else {
        battery_status = ERROR_FLAG_SENSOR_FAULT;
    }

    s_batt.status = battery_status;

    /* Build & send CAN frame */
    BatteryStatus_t f;
    f.voltage_mv   = s_batt.voltage_mv;
    f.current_ma   = s_batt.current_ma;
    f.soc          = s_batt.soc;
    f.temperature  = s_batt.temperature;
    f.cycles       = s_batt.cycles;
    f.status       = s_batt.status;

    mcp_send_message(CAN_ID_BATTERY, (uint8_t*)&f, sizeof(f));

    /* Log */
    if (s_batt.ina_status == HAL_OK && s_batt.ina.valid) {
        sys_log(ctx, "[Battery] %.2fV | %.2fA | %.2fW | SOC=%u%% | Status=0x%02X\r\n",
                s_batt.ina.voltage_V, s_batt.ina.current_A, s_batt.ina.power_W,
                (unsigned)s_batt.soc, (unsigned)s_batt.status);
    } else {
        sys_log(ctx, "[Battery] SENSOR ERROR | Status=0x%02X\r\n", (unsigned)s_batt.status);
    }

    tx_thread_sleep(CAN_PERIOD_BATTERY_MS);
}
