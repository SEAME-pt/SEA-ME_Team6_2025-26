#pragma once

#include <stdint.h>
#include "tx_api.h"
#include "i2c.h"   // hi2c1, etc (or forward declare)
#include "system_ctx.h"  // your SystemCtx + VehicleState

void task_srf08_init(SystemCtx* ctx);
void task_srf08_step(SystemCtx* ctx);
