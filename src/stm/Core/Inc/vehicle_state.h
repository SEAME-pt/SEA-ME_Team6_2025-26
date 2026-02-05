#pragma once
#include <stdint.h>

typedef struct {
    uint32_t rpm;
    float    tof_m;
    float    mag_uT[3];

    uint32_t rpm_ts;
    uint32_t tof_ts;
    uint32_t mag_ts;
} VehicleState;
