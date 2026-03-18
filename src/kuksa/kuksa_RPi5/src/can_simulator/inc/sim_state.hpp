#pragma once

#include <cstdint>
#include "../../../inc/can_id.h"

struct SimState {
  // Heartbeat
  uint8_t hb_state  = SYSTEM_STATE_RUNNING;
  uint8_t hb_errors = 0;
  uint8_t hb_mode   = DRIVE_MODE_MANUAL;

  // Emergency stop
  uint8_t  estop_active = 0;
  uint8_t  estop_source = 0;
  uint16_t estop_mm     = 4000;
  uint8_t  estop_reason = 0;

  // Wheel speed
  int16_t  rpm = 0;
  uint32_t pulses = 0;
  uint8_t  direction = 0;
  uint8_t  wheel_status = 0;

  // ToF
  uint16_t tof_mm = 4000;
  uint8_t  tof_zone = 255;
  uint8_t  tof_target_status = 0;
  uint8_t  tof_count = 0;
  uint8_t  tof_status = 0;

  // Environment
  float    env_temp_c = 21.5f;
  float    env_humidity = 50.0f;
  uint32_t env_light_lux = 800;
  float    env_pressure_hpa = 1013.25f;
  uint8_t  env_status = 0;

  // Battery
  uint16_t batt_mv = 12300;
  int16_t  batt_ma = 0;
  uint8_t  batt_soc = 85;
  int8_t   batt_temp_c = 25;
  uint8_t  batt_cycles = 0;
  uint8_t  batt_status = 0;

  // IMU
  float acc_gx = 0.0f, acc_gy = 0.0f, acc_gz = 1.0f;
  float gyro_dpsx = 0.0f, gyro_dpsy = 0.0f, gyro_dpsz = 0.0f;
  float mag_mgx = 0.0f, mag_mgy = 0.0f, mag_mgz = 0.0f;
  uint8_t imu_status = 0;

  // Motor
  int8_t   motor_throttle = 0;
  int8_t   motor_steering = 0;
  uint16_t motor_current_ma = 0;
  int8_t   motor_driver_temp = 30;
  uint8_t  motor_pwm = 0;
};