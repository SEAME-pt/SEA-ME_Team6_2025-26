#pragma once

#include "system_ctx.h"
#include "can_id.h"
#include "tx_api.h"
#include "sys_helpers.h"
#include "can_tx.h"
#include "motor_control.h"
#include "emergency_stop.h"
#include "srf08.h"
#include "i2c.h"

#include "main.h" 

void task_srf08_init(SystemCtx* ctx);
void task_srf08_step(SystemCtx* ctx);
