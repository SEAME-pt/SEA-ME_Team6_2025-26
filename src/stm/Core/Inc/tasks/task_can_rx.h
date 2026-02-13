#pragma once
#include "tx_api.h"
#include "system_ctx.h"

void task_can_rx_init(SystemCtx* ctx);
void task_can_rx_step(SystemCtx* ctx);
