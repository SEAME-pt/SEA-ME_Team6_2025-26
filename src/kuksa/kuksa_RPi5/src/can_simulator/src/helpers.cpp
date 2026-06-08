#include "../inc/helpers.hpp"

uint8_t crc8(const uint8_t* data, size_t len)
{
  uint8_t crc = 0x00;
  for (size_t i = 0; i < len; ++i) {
    crc ^= data[i];
    for (int b = 0; b < 8; ++b) {
      if (crc & 0x80) crc = static_cast<uint8_t>((crc << 1) ^ 0x07);
      else           crc = static_cast<uint8_t>(crc << 1);
    }
  }
  return crc;
}

uint64_t now_ms()
{
  using namespace std::chrono;
  return std::chrono::duration_cast<std::chrono::milliseconds>(
           std::chrono::steady_clock::now().time_since_epoch()).count();
}

int16_t clamp_i16(long v) {
  if (v > 32767) return 32767;
  if (v < -32768) return -32768;
  return static_cast<int16_t>(v);
}
uint8_t clamp_u8(long v) {
  if (v < 0) return 0;
  if (v > 255) return 255;
  return static_cast<uint8_t>(v);
}

std::string trim(const std::string& s)
{
  size_t b = 0;
  while (b < s.size() && std::isspace((unsigned char)s[b])) b++;
  size_t e = s.size();
  while (e > b && std::isspace((unsigned char)s[e-1])) e--;
  return s.substr(b, e-b);
}

bool split_kv(const std::string& token, std::string& k, std::string& v)
{
  const size_t eq = token.find('=');
  if (eq == std::string::npos) return false;
  k = trim(token.substr(0, eq));
  v = trim(token.substr(eq+1));
  return !k.empty();
}

void apply_kv(SimState& st, const std::string& k, const std::string& v)
{
  auto to_l = [&](const std::string& s)->long { return std::strtol(s.c_str(), nullptr, 10); };
  auto to_ul = [&](const std::string& s)->unsigned long { return std::strtoul(s.c_str(), nullptr, 10); };
  auto to_f = [&](const std::string& s)->float { return std::strtof(s.c_str(), nullptr); };

  // Heartbeat
  if (k == "hb_state") st.hb_state = clamp_u8((long)to_ul(v));
  else if (k == "hb_errors") st.hb_errors = clamp_u8((long)to_ul(v));
  else if (k == "hb_mode") st.hb_mode = clamp_u8((long)to_ul(v));

  // Emergency stop
  else if (k == "estop_active") st.estop_active = (clamp_u8((long)to_ul(v)) ? 1 : 0);
  else if (k == "estop_source") st.estop_source = clamp_u8((long)to_ul(v));
  else if (k == "estop_mm") st.estop_mm = static_cast<uint16_t>(to_ul(v));
  else if (k == "estop_reason") st.estop_reason = clamp_u8((long)to_ul(v));

  // Wheel speed
  else if (k == "rpm") st.rpm = clamp_i16(to_l(v));
  else if (k == "pulses") st.pulses = static_cast<uint32_t>(to_ul(v));
  else if (k == "direction") st.direction = clamp_u8((long)to_ul(v));
  else if (k == "wheel_status") st.wheel_status = clamp_u8((long)to_ul(v));

  // ToF
  else if (k == "tof_mm") st.tof_mm = static_cast<uint16_t>(to_ul(v));
  else if (k == "tof_zone") st.tof_zone = clamp_u8((long)to_ul(v));
  else if (k == "tof_ts") st.tof_target_status = clamp_u8((long)to_ul(v));
  else if (k == "tof_count") st.tof_count = clamp_u8((long)to_ul(v));
  else if (k == "tof_status") st.tof_status = clamp_u8((long)to_ul(v));

  // Environment
  else if (k == "env_temp_c") st.env_temp_c = to_f(v);
  else if (k == "env_humidity") st.env_humidity = to_f(v);
  else if (k == "env_light_lux") st.env_light_lux = static_cast<uint32_t>(to_ul(v));
  else if (k == "env_pressure_hpa") st.env_pressure_hpa = to_f(v);
  else if (k == "env_status") st.env_status = clamp_u8((long)to_ul(v));

  // Battery
  else if (k == "batt_mv") st.batt_mv = static_cast<uint16_t>(to_ul(v));
  else if (k == "batt_ma") st.batt_ma = clamp_i16(to_l(v));
  else if (k == "batt_soc") st.batt_soc = clamp_u8((long)to_ul(v));
  else if (k == "batt_temp_c") st.batt_temp_c = static_cast<int8_t>(to_l(v));
  else if (k == "batt_cycles") st.batt_cycles = clamp_u8((long)to_ul(v));
  else if (k == "batt_status") st.batt_status = clamp_u8((long)to_ul(v));

  // IMU
  else if (k == "acc_gx") st.acc_gx = to_f(v);
  else if (k == "acc_gy") st.acc_gy = to_f(v);
  else if (k == "acc_gz") st.acc_gz = to_f(v);
  else if (k == "gyro_dpsx") st.gyro_dpsx = to_f(v);
  else if (k == "gyro_dpsy") st.gyro_dpsy = to_f(v);
  else if (k == "gyro_dpsz") st.gyro_dpsz = to_f(v);
  else if (k == "mag_mgx") st.mag_mgx = to_f(v);
  else if (k == "mag_mgy") st.mag_mgy = to_f(v);
  else if (k == "mag_mgz") st.mag_mgz = to_f(v);
  else if (k == "imu_status") st.imu_status = clamp_u8((long)to_ul(v));

  // Motor status
  else if (k == "motor_throttle") st.motor_throttle = static_cast<int8_t>(to_l(v));
  else if (k == "motor_steering") st.motor_steering = static_cast<int8_t>(to_l(v));
  else if (k == "motor_current_ma") st.motor_current_ma = static_cast<uint16_t>(to_ul(v));
  else if (k == "motor_driver_temp") st.motor_driver_temp = static_cast<int8_t>(to_l(v));
  else if (k == "motor_pwm") st.motor_pwm = clamp_u8((long)to_ul(v));

  else {
    std::cerr << "[EMU] Unknown key: " << k << "\n";
  }
}