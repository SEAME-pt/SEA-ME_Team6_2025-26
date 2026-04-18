#pragma once

#include "../system_ctx.h"
#include "../sys_helpers.h"
#include "../cruise_control.h"
#include <stdint.h>
#include "tx_api.h"

/* Global CC instance — also accessed by task_can_rx for command dispatch */
extern CruiseControl_t g_cruise_control;

/* Mutex protecting g_cruise_control — take before any read/write of the struct */
extern TX_MUTEX g_cc_mutex;

void task_cc_init(SystemCtx *ctx);
void task_cc_step(SystemCtx *ctx);
