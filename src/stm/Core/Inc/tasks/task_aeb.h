#pragma once
#include "../system_ctx.h"
#include "../sys_helpers.h"
#include <stdint.h>
#include "tx_api.h"
#include <stdbool.h>
#include "can_tx.h"
#include "can_id.h"
#include <stdio.h>

void task_aeb_init(SystemCtx* ctx);

/**
 * @brief Periodic AEB step
 * @param ctx shared context
 * @param dt_ms time since last call (ms)
 */
void task_aeb_step(SystemCtx* ctx, uint32_t dt_ms);
