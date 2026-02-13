#pragma once

#include <stdint.h>
#include "tx_api.h"
#include "i2c.h"   // hi2c1, etc (or forward declare)
#include "system_ctx.h"  // your SystemCtx + VehicleState
#include "srf08.h"  // SRF08_HandleTypeDef, SRF08_Init(), etc
#include "can_id.h" // CAN_ID_*
#include "motor_control.h" // Motor_Stop()
#include "sys_helpers.h"   // calculate_crc8()
#include "can_tx.h"        // mcp_send_message()
#include "emergency_stop.h" 

void task_srf08_init(SystemCtx* ctx);
void task_srf08_step(SystemCtx* ctx);
