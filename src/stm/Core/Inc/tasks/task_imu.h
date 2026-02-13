#pragma once

#include "system_ctx.h"
#include "can_id.h"          // CAN_ID_IMU_*, ImuAccel_t, ImuGyro_t, ImuMag_t
#include "tx_api.h"
#include "sys_helpers.h"     // sys_log(...)
#include "can_tx.h"          // mcp_send_message(...)
#include "ism330dhcx.h"
#include "iis2mdc.h"        

void task_imu_init(SystemCtx* ctx);
void task_imu_step(SystemCtx* ctx);