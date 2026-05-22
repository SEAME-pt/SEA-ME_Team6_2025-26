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

/**
 * @brief Force-reset the AEB state machine to OFF.
 *
 * Called when the ADAS Manager explicitly clears the emergency stop
 * (EmergencyStop_t with active=0).  This releases the AEB LATCH so
 * the vehicle can resume movement under Manager control.
 *
 * Safety: the AEB step runs every 20 ms and will immediately re-arm and
 * potentially re-latch on the very next cycle if an obstacle is still
 * present, so this reset is safe to perform unconditionally.
 *
 * @param ctx shared context
 */
void task_aeb_force_reset(SystemCtx* ctx);
