#pragma once

#include "system_ctx.h"
#include "can_id.h"
#include "tx_api.h"
#include "sys_helpers.h"
#include "can_tx.h"
#include "vl53l5cx_api.h"

#include "main.h" 

extern volatile uint8_t vlx_ready;

void task_tof_init(SystemCtx* ctx);
void task_tof_step(SystemCtx* ctx);
