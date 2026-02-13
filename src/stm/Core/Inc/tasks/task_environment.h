#pragma once

#include "tx_api.h"
#include <stdio.h>

#include "main.h"               // HAL GPIO, hi2c1/hi2c2, HAL types
#include "mcp2515.h"            // mcp_init
#include "can_id.h"             // CAN_ID_HEARTBEAT_STM32, Heartbeat_t
#include "system_ctx.h"         // SystemCtx, system_ctx()
#include "sys_helpers.h"        // sys_log, calculate_crc8
#include "lps22hh.h"
#include "hts221.h"
#include "veml6030.h"
#include "can_tx.h"             // mcp_send_message
#include "can_id.h"             // CAN_ID_ENVIRONMENT

void task_environment_init(SystemCtx* ctx);
void task_environment_step(SystemCtx* ctx);

