#pragma once
#include "system_ctx.h"
#include "can_id.h"

#include "tx_api.h"
#include "sys_helpers.h"     // sys_log
#include "can_tx.h"        // mcp_send_message

#include "i2c.h"                  // hi2c1
#include "ina226.h"                // INA226_*

#include <stdlib.h>                // abs

void task_battery_init(SystemCtx* ctx);
void task_battery_step(SystemCtx* ctx);
