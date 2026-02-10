#include "./tasks/task_heartbeat.h"
#include "mcp2515.h"
#include "mcp2515.h"
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

void task_heartbeat_init(SystemCtx* ctx)
{
    (void)ctx;

    sys_log(ctx, "\r\n[HeartBeat] Thread iniciada!\r\n");

    /* Small settle time before SPI/CAN init */
    tx_thread_sleep(10);

    /* Init MCP2515 once */
    mcp_init(ctx);
    sys_log(ctx, "[HeartBeat] MCP2515 inicializado\r\n");

    tx_mutex_get(&ctx->sys_mutex, TX_WAIT_FOREVER);
    ctx->system_state = SYSTEM_STATE_READY;
    tx_mutex_put(&ctx->sys_mutex);

    tx_thread_sleep(100);

    tx_mutex_get(&ctx->sys_mutex, TX_WAIT_FOREVER);
    ctx->system_state = SYSTEM_STATE_RUNNING;
    tx_mutex_put(&ctx->sys_mutex);
}

void task_heartbeat_step(SystemCtx* ctx)
{
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
    HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);

    sys_log(ctx,
        "[HeartBeat] State=%u | Uptime=%lu ms | Errors=0x%02X | Mode=%u | CRC=0x%02X\r\n",
        hb_frame.state,
        (unsigned long)hb_frame.uptime_ms,
        hb_frame.errors,
        hb_frame.mode,
        hb_frame.crc
    );
}
