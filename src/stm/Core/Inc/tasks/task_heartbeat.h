#pragma once

#include "tx_api.h"
#include <stdio.h>

#include "main.h"               // HAL_GetTick, GPIO pins
#include "mcp2515.h"            // mcp_init, mcp_send_message
#include "can_ids.h"            // CAN_ID_HEARTBEAT_STM32, Heartbeat_t
#include "system_ctx.h"         // SystemCtx, system_ctx()
#include "sys_helpers.h"        // sys_log, calculate_crc8

void task_heartbeat_init(SystemCtx* ctx);
void task_heartbeat_step(SystemCtx* ctx);
