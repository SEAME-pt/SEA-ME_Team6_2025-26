#pragma once
#include "tx_api.h"
#include "system_ctx.h"

void task_can_rx_init(SystemCtx* ctx);
void task_can_rx_step(SystemCtx* ctx);

/* Valores realmente aplicados ao servo/motor — para o CtrlStatus_t (0x213) */
int8_t task_can_rx_actual_steering(void);
int8_t task_can_rx_actual_throttle(void);
