#include "task_heartbeat.h"

/* globals for now (not in ctx yet) */
// WORK IN PROGRESS - DAVID
// These will be updated by other threads, and updated in the ctx snapshot pattern later
// Then this thread should read from ctx snapshot instead of globals
extern volatile uint8_t current_system_state;
extern volatile uint8_t current_drive_mode;
extern volatile uint8_t system_error_flags;

void task_heartbeat_init(SystemCtx* ctx)
{
    (void)ctx;

    hb_log(ctx, "\r\n[HeartBeat] Thread iniciada!\r\n");

    /* Small settle time before SPI/CAN init */
    tx_thread_sleep(10);

    /* Init MCP2515 once */
    mcp_init();
    hb_log(ctx, "[HeartBeat] MCP2515 inicializado\r\n");

    /* Same state progression as before (still global for now) */
    current_system_state = SYSTEM_STATE_READY;

    tx_thread_sleep(100);

    current_system_state = SYSTEM_STATE_RUNNING;
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

    hb_log(ctx,
        "[HeartBeat] State=%u | Uptime=%lu ms | Errors=0x%02X | Mode=%u | CRC=0x%02X\r\n",
        hb_frame.state,
        (unsigned long)hb_frame.uptime_ms,
        hb_frame.errors,
        hb_frame.mode,
        hb_frame.crc
    );
}
