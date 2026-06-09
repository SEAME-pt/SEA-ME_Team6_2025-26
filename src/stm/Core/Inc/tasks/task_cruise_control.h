#pragma once

#include "../system_ctx.h"
#include "../sys_helpers.h"
#include "../cruise_control.h"
#include "../can_protocol.h"
#include <stdint.h>
#include "tx_api.h"

/* Global CC instance — also accessed by task_can_rx for command dispatch */
extern CruiseControl_t g_cruise_control;

/* Latest longitudinal command received via 0x202.
 * Written by task_can_rx (handle_ctrl_cmd), read by task_cc_step.
 * Both accesses must hold g_cc_mutex. */
typedef struct {
    uint8_t  mode;          /* CtrlMode_t — last received mode */
    float    target_kmh;    /* converted from target_speed_cms (only valid in CC/ACC) */
    uint8_t  headway;       /* HeadwaySetting_t (only valid in ACC) */
    uint8_t  aeb_request;   /* Manager-side AEB pulse */
    uint32_t last_rx_ms;    /* HAL_GetTick() of last update — for staleness */
} LongitudinalCmd_t;

extern LongitudinalCmd_t g_long_cmd;

/* Mutex protecting g_cruise_control AND g_long_cmd — take before any read/write */
extern TX_MUTEX g_cc_mutex;

void task_cc_init(SystemCtx *ctx);
void task_cc_step(SystemCtx *ctx);
