#pragma once
#include <stdint.h>

typedef struct {
    uint32_t rpm;
    uint16_t tof_distance_mm;
    uint16_t srf08_distance_mm
    uint8_t  srf08_light;
    int16_t  accel_mg[3];      // x,y,z in milli-g
    int16_t  gyro_dps[3];      // x,y,z in 0.1 deg/s
    int16_t  mag_mG[3];        // x,y,z in milli-Gauss
    uint32_t speed_mh;         // meters per hour

    uint32_t srf08_ts;
    uint32_t speed_ts;
    uint32_t rpm_ts;
    uint32_t tof_ts;
    uint32_t imu_ts;
} VehicleState;
