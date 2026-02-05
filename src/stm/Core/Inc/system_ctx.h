#pragma once

#include "tx_api.h"
#include "vehicle_state.h" 

typedef struct {
  TX_MUTEX printf_mutex;

  TX_MUTEX state_mutex;    // for shared VehicleState snapshot pattern
  VehicleState state;      // rpm, tof, mag for now

  // later:
  // TX_EVENT_FLAGS_GROUP data_flags;
} SystemCtx;

SystemCtx* system_ctx(void);
