// task_speed.h
#pragma once
#include "system_ctx.h"
#include "tx_api.h"
#include "sys_helpers.h"        // sys_log(...)
#include "can_tx.h"             // mcp_send_message(...)
#include "can_id.h"             // CAN_ID_WHEEL_SPEED, WheelSpeed_t
#include "speedometer.h"        // Speedometer_*

void task_speed_init(SystemCtx* ctx);
void task_speed_step(SystemCtx* ctx);
