#pragma once
#include <stdint.h>

typedef struct {
    uint32_t rpm;
    float    tof_m;
    int16_t accel_mg[3];      // x,y,z in milli-g
    int16_t gyro_dps[3];      // x,y,z in 0.1 deg/s
    int16_t mag_mG[3];        // x,y,z in milli-Gauss
    uint32_t speed_mh;        // meters per hour

    uint32_t speed_ts;
    uint32_t rpm_ts;
    uint32_t tof_ts;
    uint32_t mag_ts;
} VehicleState;
