#pragma once
#include <stdint.h>

typedef struct {
    uint32_t rpm;
    uint16_t tof_distance_mm;
    uint16_t srf08_distance_mm;
    uint8_t  srf08_light;
    uint8_t  srf08_valid;
    uint8_t  srf08_speed_limit;
    uint8_t  emergency_stop_active;
    int16_t  accel_mg[3];      // x,y,z in milli-g
    int16_t  gyro_dps[3];      // x,y,z in 0.1 deg/s
    int16_t  mag_mG[3];        // x,y,z in milli-Gauss
    uint32_t speed_mh;         // meters per hour

    // AEB
    uint8_t  aeb_stop_active;     // 1 = AEB forcing stop
    uint8_t  aeb_warn;            // optional warning flag
    uint8_t  aeb_state;           // enum for debugging
    uint16_t aeb_ttc_ms;          // debug
    uint16_t aeb_dstop_mm;        // debug
    uint32_t aeb_ts;              // last AEB update (ms)
    uint8_t  aeb_speed_limit;     // Speed limit from AEB (0-100%)

    uint32_t srf08_ts;
    uint32_t speed_ts;
    uint32_t rpm_ts;
    uint32_t tof_ts;
    uint32_t imu_ts;
} VehicleState;
