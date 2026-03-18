#include "../inc/frames_builder.hpp"

// -----------------------------
// Build frames from SimState using can_id.h structs
// -----------------------------
WheelSpeed_t make_wheel(const SimState& st)
{
  WheelSpeed_t w{};
  w.rpm          = st.rpm;
  w.total_pulses = st.pulses;
  w.direction    = st.direction;
  w.status       = st.wheel_status;
  return w;
}

Heartbeat_t make_hb(const SimState& st, uint32_t uptime_ms)
{
  Heartbeat_t h{};
  h.state     = st.hb_state;
  h.uptime_ms = uptime_ms;
  h.errors    = st.hb_errors;
  h.mode      = st.hb_mode;
  h.crc       = 0;
  h.crc       = crc8(reinterpret_cast<const uint8_t*>(&h), sizeof(h) - 1);
  return h;
}

Environment_t make_env(const SimState& st)
{
  Environment_t e{};
  const long temp_x100 = std::lround(st.env_temp_c * 100.0f);
  e.temperature = clamp_i16(temp_x100);

  float hum = st.env_humidity;
  if (hum < 0.0f) hum = 0.0f;
  if (hum > 100.0f) hum = 100.0f;
  e.humidity = static_cast<uint8_t>(std::lround(hum));

  uint32_t amb = st.env_light_lux / 100U;
  if (amb > 255U) amb = 255U;
  e.ambient_light_x100 = static_cast<uint8_t>(amb);

  const uint32_t pressure_pa = static_cast<uint32_t>(std::lround(st.env_pressure_hpa * 100.0f)); // hPa -> Pa
  e.pressure = (pressure_pa & 0xFFFFFFu);

  e.status = st.env_status;
  return e;
}

BatteryStatus_t make_batt(const SimState& st)
{
  BatteryStatus_t b{};
  b.voltage_mv  = st.batt_mv;
  b.current_ma  = st.batt_ma;
  b.soc         = st.batt_soc;
  b.temperature = st.batt_temp_c;
  b.cycles      = st.batt_cycles;
  b.status      = st.batt_status;
  return b;
}

ImuAccel_t make_accel(const SimState& st)
{
  ImuAccel_t a{};
  a.acc_x = clamp_i16(std::lround(st.acc_gx * 1000.0f));
  a.acc_y = clamp_i16(std::lround(st.acc_gy * 1000.0f));
  a.acc_z = clamp_i16(std::lround(st.acc_gz * 1000.0f));
  a.reserved = 0;
  a.status   = st.imu_status;
  return a;
}

ImuGyro_t make_gyro(const SimState& st)
{
  ImuGyro_t g{};
  g.gyro_x = clamp_i16(std::lround(st.gyro_dpsx * 10.0f));
  g.gyro_y = clamp_i16(std::lround(st.gyro_dpsy * 10.0f));
  g.gyro_z = clamp_i16(std::lround(st.gyro_dpsz * 10.0f));
  g.reserved = 0;
  g.status   = st.imu_status;
  return g;
}

ImuMag_t make_mag(const SimState& st)
{
  ImuMag_t m{};
  m.mag_x = clamp_i16(std::lround(st.mag_mgx));
  m.mag_y = clamp_i16(std::lround(st.mag_mgy));
  m.mag_z = clamp_i16(std::lround(st.mag_mgz));
  m.reserved = 0;
  m.status   = st.imu_status;
  return m;
}

ToFDistance_t make_tof(const SimState& st)
{
  ToFDistance_t t{};
  t.min_distance_mm = st.tof_mm;
  t.nearest_zone    = st.tof_zone;
  t.target_status   = st.tof_target_status;
  t.detection_count = st.tof_count;
  t.reserved[0] = 0;
  t.reserved[1] = 0;
  t.status = st.tof_status;
  return t;
}

EmergencyStop_t make_estop(const SimState& st)
{
  EmergencyStop_t e{};
  e.active      = st.estop_active;
  e.source      = st.estop_source;
  e.distance_mm = st.estop_mm;
  e.reason      = st.estop_reason;
  e.reserved[0] = 0;
  e.reserved[1] = 0;
  e.crc         = 0;
  e.crc         = crc8(reinterpret_cast<const uint8_t*>(&e), sizeof(e) - 1);
  return e;
}

MotorStatus_t make_motor_status(const SimState& st, uint8_t counter)
{
  MotorStatus_t m{};
  m.actual_throttle  = st.motor_throttle;
  m.actual_steering  = st.motor_steering;
  m.motor_current_ma = st.motor_current_ma;
  m.driver_temp      = st.motor_driver_temp;
  m.pwm_duty         = st.motor_pwm;
  m.counter          = counter;
  m.crc              = 0;
  m.crc              = crc8(reinterpret_cast<const uint8_t*>(&m), sizeof(m) - 1);
  return m;
}