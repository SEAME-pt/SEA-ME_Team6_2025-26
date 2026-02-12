#pragma once

#include "tx_api.h"
#include "vehicle_state.h" 

typedef struct {
  TX_MUTEX printf_mutex;
  TX_MUTEX i2c1_mutex;

  TX_MUTEX state_mutex;    // for shared VehicleState snapshot pattern
  VehicleState state;      // rpm, tof, mag for now

  TX_MUTEX sys_mutex;           // protects system fields below
  uint8_t system_state;         // SYSTEM_STATE_*
  uint8_t drive_mode;           // DRIVE_MODE_*
  uint8_t error_flags;          // bitmask
} SystemCtx;

SystemCtx* system_ctx(void);
void system_ctx_init(void);
