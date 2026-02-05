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

extern SPI_HandleTypeDef hspi1;

/* globals for now (not in ctx yet) */
// WORK IN PROGRESS - DAVID
// These will be updated by other threads, and updated in the ctx snapshot pattern later
// Then this thread should read from ctx snapshot instead of globals
extern volatile uint8_t current_system_state;
extern volatile uint8_t current_drive_mode;
extern volatile uint8_t system_error_flags;

static void mcp_send_message(uint16_t id, uint8_t *data, uint8_t len)
{
    uint8_t buf[6 + 8];  // 5 header + DLC + up to 8 data bytes

    buf[0] = MCP_LOAD_TX;                 // 0x40 command
    buf[1] = (id >> 3) & 0xFF;            // SIDH
    buf[2] = (id & 0x07) << 5;            // SIDL
    buf[3] = 0x00;                        // EID8 (not used)
    buf[4] = 0x00;                        // EID0 (not used)
    buf[5] = len & 0x0F;                  // DLC (0–8)

    // Copy payload
    for (uint8_t i = 0; i < len; i++)
        buf[6 + i] = data[i];

    // Send SPI frame
    MCP_CS_L();
    HAL_SPI_Transmit(&hspi1, buf, 6 + len, HAL_MAX_DELAY);
    MCP_CS_H();

    // Request-to-send TX buffer 0
    uint8_t cmd = MCP_RTS_TXB0;
    MCP_CS_L();
    HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
    MCP_CS_H();
}

void task_heartbeat_init(SystemCtx* ctx)
{
    (void)ctx;

    sys_log(ctx, "\r\n[HeartBeat] Thread iniciada!\r\n");

    /* Small settle time before SPI/CAN init */
    tx_thread_sleep(10);

    /* Init MCP2515 once */
    mcp_init();
    sys_log(ctx, "[HeartBeat] MCP2515 inicializado\r\n");

    uint8_t st, err, mode;
    /* Same state progression as before (still global for now) */
    tx_mutex_get(&ctx->sys_mutex, TX_WAIT_FOREVER);
	st   = ctx->system_state;
	tx_mutex_put(&ctx->sys_mutex);
    // current_system_state = SYSTEM_STATE_READY;

    tx_thread_sleep(100);

    // current_system_state = SYSTEM_STATE_RUNNING;
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
