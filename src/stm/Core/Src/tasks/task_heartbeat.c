#include "./tasks/task_heartbeat.h"
#include "mcp2515.h"
#include "mcp2515.h"
#include "stm32u5xx_hal.h"   /* for IWDG HAL */
#include "lps22hh.h"
#include "speedometer.h"
#include "ism330dhcx.h"
#include "iis2mdc.h"
#include "veml6030.h"
#include "vl53l5cx_driver.h"
#include "hts221.h"
#include "main.h"
#include <stdio.h>
#include "can_id.h"
#include "thread_config.h"
#include "lcd1602.h"
#include "motor_control.h"
#include "servo.h"
#include "tim.h"
#include "emergency_stop.h"
#include "srf08.h"
#include "ina226.h"
#include "app_threadx.h"

#include "can_tx.h"

extern SPI_HandleTypeDef hspi1;

/*
 * IWDG — Independent Watchdog
 *
 * Clock source: LSI ~32 kHz (always-on, independent of main clock)
 * Prescaler /32  →  32000 / 32 = 1000 Hz  →  1 ms per tick
 * Reload = 1999  →  timeout = 2000 ms = 2 s
 *
 * The heartbeat thread feeds the watchdog every CAN_PERIOD_HEARTBEAT_MS (100 ms).
 * If the scheduler stops, a hard fault occurs, or all threads block on a deadlock,
 * the watchdog fires after 2 s and resets the MCU automatically — no physical
 * Reset button needed.
 *
 * WARNING: once HAL_IWDG_Init() is called the watchdog CANNOT be stopped.
 * HAL_IWDG_Refresh() must be called at least every 2 s while the system runs.
 */
static IWDG_HandleTypeDef s_hiwdg;

void task_heartbeat_init(SystemCtx* ctx)
{
    (void)ctx;

    sys_log(ctx, "[HeartBeat] Thread iniciada!");

    /* Small settle time before SPI/CAN init */
    tx_thread_sleep(10);

    /* Init MCP2515 once */
    mcp_init(ctx);
    sys_log(ctx, "[HeartBeat] MCP2515 inicializado");

    tx_mutex_get(&ctx->sys_mutex, TX_WAIT_FOREVER);
    ctx->system_state = SYSTEM_STATE_READY;
    tx_mutex_put(&ctx->sys_mutex);

    tx_thread_sleep(100);

    tx_mutex_get(&ctx->sys_mutex, TX_WAIT_FOREVER);
    ctx->system_state = SYSTEM_STATE_RUNNING;
    tx_mutex_put(&ctx->sys_mutex);

    /* Start the IWDG watchdog — timeout ~2 s (LSI/32, reload=1999) */
    s_hiwdg.Instance       = IWDG;
    s_hiwdg.Init.Prescaler = IWDG_PRESCALER_32;
    s_hiwdg.Init.Reload    = 1999U;            /* 2000 ms @ 1 kHz (LSI/32) */
    s_hiwdg.Init.Window    = IWDG_WINDOW_DISABLE;
    if (HAL_IWDG_Init(&s_hiwdg) == HAL_OK) {
        sys_log(ctx, "[HeartBeat] IWDG watchdog ON — timeout 2 s, auto-reset se congelado");
    } else {
        sys_log(ctx, "[HeartBeat] IWDG init FALHOU — sem watchdog!");
    }
}

void task_heartbeat_step(SystemCtx* ctx)
{
    /* Feed IWDG first — must happen at least every 2 s */
    HAL_IWDG_Refresh(&s_hiwdg);

    Heartbeat_t hb_frame;

    uint8_t st, err, mode;

    tx_mutex_get(&ctx->sys_mutex, TX_WAIT_FOREVER);
    st   = ctx->system_state;
    err  = ctx->error_flags;
    mode = ctx->drive_mode;
    tx_mutex_put(&ctx->sys_mutex);

    hb_frame.state     = st;
    hb_frame.uptime_ms = HAL_GetTick();
    hb_frame.errors    = err;
    hb_frame.mode      = mode;

    /* CRC over all bytes except crc field (last byte) */
    hb_frame.crc = calculate_crc8((uint8_t*)&hb_frame, sizeof(hb_frame) - 1);

    /* Send CAN frame */
    mcp_send_message(CAN_ID_HEARTBEAT_STM32, (uint8_t*)&hb_frame, sizeof(hb_frame));

    /* Visual heartbeat */
    //HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);

    /*
    sys_log(ctx,
        "[HeartBeat] State=%u | Uptime=%lu ms | Errors=0x%02X | Mode=%u | CRC=0x%02X",
        hb_frame.state,
        (unsigned long)hb_frame.uptime_ms,
        hb_frame.errors,
        hb_frame.mode,
        hb_frame.crc
    );
    */
}
