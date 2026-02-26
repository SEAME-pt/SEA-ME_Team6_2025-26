// tools/can_sim/can_emulator.cpp
//
// Interactive CAN-only emulator that mimics STM32 publishing on SocketCAN.

#include <cerrno>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>
#include <cmath>
#include <mutex>
#include <atomic>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>

#if __has_include(<filesystem>)
  #include <filesystem>
  namespace fs = std::filesystem;
#else
  #error "C++17 <filesystem> required"
#endif

#include "../../inc/can_id.h"
#include "inc/helpers.hpp"
#include "inc/sim_state.hpp"
#include "inc/app_context.hpp"

static uint8_t crc8(const uint8_t* data, size_t len)
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

// -----------------------------
// SocketCAN helpers
// -----------------------------
static int open_can_tx_socket(const std::string& ifname)
{
  int s = ::socket(PF_CAN, SOCK_RAW, CAN_RAW);
  if (s < 0) {
    std::cerr << "[EMU] socket() failed: " << std::strerror(errno) << "\n";
    return -1;
  }

  ifreq ifr{};
  std::strncpy(ifr.ifr_name, ifname.c_str(), IFNAMSIZ - 1);

  if (::ioctl(s, SIOCGIFINDEX, &ifr) < 0) {
    std::cerr << "[EMU] ioctl(SIOCGIFINDEX) failed: " << std::strerror(errno) << "\n";
    ::close(s);
    return -1;
  }

  sockaddr_can addr{};
  addr.can_family  = AF_CAN;
  addr.can_ifindex = ifr.ifr_ifindex;

  if (::bind(s, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
    std::cerr << "[EMU] bind() failed: " << std::strerror(errno) << "\n";
    ::close(s);
    return -1;
  }

  std::cout << "[EMU] Sending on interface: " << ifname << "\n";
  return s;
}

static bool send_payload8(int sock, uint32_t can_id, const void* payload8)
{
  can_frame f{};
  f.can_id  = can_id;
  f.can_dlc = 8;
  std::memcpy(f.data, payload8, 8);
  const ssize_t n = ::write(sock, &f, sizeof(f));
  if (n != static_cast<ssize_t>(sizeof(f))) {
    std::cerr << "[EMU] write() failed/short: " << std::strerror(errno) << "\n";
    return false;
  }
  return true;
}

// -----------------------------
// Sim state (latest values)
// -----------------------------
/*
struct SimState {
  // Heartbeat
  uint8_t hb_state  = SYSTEM_STATE_RUNNING;
  uint8_t hb_errors = 0;
  uint8_t hb_mode   = DRIVE_MODE_MANUAL;

  // Emergency stop
  uint8_t  estop_active = 0;
  uint8_t  estop_source = 0; // 0=ToF, 1=AGL, 2=Manual, 3=Watchdog
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

  // Environment (semantic)
  float    env_temp_c = 21.5f;
  float    env_humidity = 50.0f;
  uint32_t env_light_lux = 800;
  float    env_pressure_hpa = 1013.25f;
  uint8_t  env_status = 0;

  // Battery (raw)
  uint16_t batt_mv = 12300;
  int16_t  batt_ma = 0;
  uint8_t  batt_soc = 85;
  int8_t   batt_temp_c = 25;
  uint8_t  batt_cycles = 0;
  uint8_t  batt_status = 0;

  // IMU (semantic)
  float acc_gx = 0.0f, acc_gy = 0.0f, acc_gz = 1.0f;
  float gyro_dpsx = 0.0f, gyro_dpsy = 0.0f, gyro_dpsz = 0.0f;
  float mag_mgx = 0.0f, mag_mgy = 0.0f, mag_mgz = 0.0f;
  uint8_t imu_status = 0;

  // Motor status
  int8_t   motor_throttle = 0;
  int8_t   motor_steering = 0;
  uint16_t motor_current_ma = 0;
  int8_t   motor_driver_temp = 30;
  uint8_t  motor_pwm = 0;
};*/

static int16_t clamp_i16(long v) {
  if (v > 32767) return 32767;
  if (v < -32768) return -32768;
  return static_cast<int16_t>(v);
}
static uint8_t clamp_u8(long v) {
  if (v < 0) return 0;
  if (v > 255) return 255;
  return static_cast<uint8_t>(v);
}

static std::string trim(const std::string& s)
{
  size_t b = 0;
  while (b < s.size() && std::isspace((unsigned char)s[b])) b++;
  size_t e = s.size();
  while (e > b && std::isspace((unsigned char)s[e-1])) e--;
  return s.substr(b, e-b);
}

static bool split_kv(const std::string& token, std::string& k, std::string& v)
{
  const size_t eq = token.find('=');
  if (eq == std::string::npos) return false;
  k = trim(token.substr(0, eq));
  v = trim(token.substr(eq+1));
  return !k.empty();
}

static void apply_kv(SimState& st, const std::string& k, const std::string& v)
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

static Scenario load_scenario_file(const fs::path& p)
{
  Scenario sc;
  sc.name = p.filename().string();
  sc.path = p.string();

  std::ifstream in(sc.path.c_str());
  if (!in) throw std::runtime_error("Failed to open scenario: " + sc.path);

  std::string line;
  uint32_t ln = 0;

  while (std::getline(in, line)) {
    ln++;
    line = trim(line);
    if (line.empty() || line[0] == '#') continue;

    // @init ...
    if (line.rfind("@init", 0) == 0) {
      std::istringstream iss(line.substr(5));
      std::string tok;
      while (iss >> tok) {
        std::string k,v;
        if (split_kv(tok, k, v)) sc.init_kv.push_back({k,v});
      }
      continue;
    }

    std::istringstream iss(line);
    std::string tok;
    Event ev;
    bool has_t = false;

    while (iss >> tok) {
      std::string k, v;
      if (!split_kv(tok, k, v)) continue;

      if (k == "t" || k == "t_ms") {
        ev.t_ms = static_cast<uint32_t>(std::strtoul(v.c_str(), nullptr, 10));
        has_t = true;
      } else {
        ev.kv.push_back({k, v});
      }
    }

    if (!has_t) {
      std::cerr << "[EMU] " << sc.name << ": line " << ln << ": missing t=..., skipping\n";
      continue;
    }
    sc.events.push_back(ev);
  }

  std::sort(sc.events.begin(), sc.events.end(),
            [](const Event& a, const Event& b){ return a.t_ms < b.t_ms; });

  if (!sc.events.empty()) sc.duration_ms = sc.events.back().t_ms;
  return sc;
}

static std::vector<Scenario> load_scenarios_dir(const std::string& dir)
{
  std::vector<Scenario> out;
  fs::path root(dir);
  if (!fs::exists(root)) throw std::runtime_error("Scenario dir does not exist: " + dir);
  if (!fs::is_directory(root)) throw std::runtime_error("Not a directory: " + dir);

  for (auto const& ent : fs::directory_iterator(root)) {
    if (!ent.is_regular_file()) continue;
    const fs::path p = ent.path();
    const std::string ext = p.extension().string();
    if (ext != ".txt" && ext != ".scn" && ext != ".scenario") continue;

    try {
      Scenario sc = load_scenario_file(p);
      if (sc.events.empty() && sc.init_kv.empty()) {
        std::cerr << "[EMU] Skipping empty scenario: " << sc.name << "\n";
        continue;
      }
      out.push_back(sc);
    } catch (const std::exception& e) {
      std::cerr << "[EMU] Failed to load " << p.filename().string() << ": " << e.what() << "\n";
    }
  }

  std::sort(out.begin(), out.end(), [](const Scenario& a, const Scenario& b){
    return a.name < b.name;
  });

  return out;
}

// -----------------------------
// Build frames from SimState using can_id.h structs
// -----------------------------
static WheelSpeed_t make_wheel(const SimState& st)
{
  WheelSpeed_t w{};
  w.rpm          = st.rpm;
  w.total_pulses = st.pulses;
  w.direction    = st.direction;
  w.status       = st.wheel_status;
  return w;
}

static Heartbeat_t make_hb(const SimState& st, uint32_t uptime_ms)
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

static Environment_t make_env(const SimState& st)
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

static BatteryStatus_t make_batt(const SimState& st)
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

static ImuAccel_t make_accel(const SimState& st)
{
  ImuAccel_t a{};
  a.acc_x = clamp_i16(std::lround(st.acc_gx * 1000.0f));
  a.acc_y = clamp_i16(std::lround(st.acc_gy * 1000.0f));
  a.acc_z = clamp_i16(std::lround(st.acc_gz * 1000.0f));
  a.reserved = 0;
  a.status   = st.imu_status;
  return a;
}

static ImuGyro_t make_gyro(const SimState& st)
{
  ImuGyro_t g{};
  g.gyro_x = clamp_i16(std::lround(st.gyro_dpsx * 10.0f));
  g.gyro_y = clamp_i16(std::lround(st.gyro_dpsy * 10.0f));
  g.gyro_z = clamp_i16(std::lround(st.gyro_dpsz * 10.0f));
  g.reserved = 0;
  g.status   = st.imu_status;
  return g;
}

static ImuMag_t make_mag(const SimState& st)
{
  ImuMag_t m{};
  m.mag_x = clamp_i16(std::lround(st.mag_mgx));
  m.mag_y = clamp_i16(std::lround(st.mag_mgy));
  m.mag_z = clamp_i16(std::lround(st.mag_mgz));
  m.reserved = 0;
  m.status   = st.imu_status;
  return m;
}

static ToFDistance_t make_tof(const SimState& st)
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

static EmergencyStop_t make_estop(const SimState& st)
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

static MotorStatus_t make_motor_status(const SimState& st, uint8_t counter)
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

// -----------------------------
// Time helpers
// -----------------------------
static uint64_t now_ms()
{
  using namespace std::chrono;
  return std::chrono::duration_cast<std::chrono::milliseconds>(
           std::chrono::steady_clock::now().time_since_epoch()).count();
}

// -----------------------------
// Global shared state
// -----------------------------
/*
static std::mutex g_state_mtx;
static SimState g_state;
static const SimState g_defaults; // value-initialized defaults

struct Playback {
  bool playing = false;
  bool loop = false;
  size_t scenario_index = 0;      // index into g_scenarios
  size_t next_event_idx = 0;
  uint64_t start_ms = 0;          // wall-clock start
};

static std::mutex g_play_mtx;
static Playback g_play;
static std::vector<Scenario> g_scenarios;
static std::string g_scenario_dir;
*/

// Apply init + any t=0 events to current state (caller holds g_state_mtx)
static void apply_scenario_start_locked(AppContext& ctx, const Scenario& sc)
{

  ctx.state = ctx.defaults;
  for (const auto& kv : sc.init_kv) apply_kv(ctx.state, kv.first, kv.second);
  // Apply t=0 events
  for (const auto& ev : sc.events) {
    if (ev.t_ms != 0) break;
    for (const auto& kv : ev.kv) apply_kv(ctx.state, kv.first, kv.second);
  }
}

// -----------------------------
// Scenario player thread
// -----------------------------
static void scenario_player_loop(AppContext& ctx)
{
  while (ctx.running.load()) {
    Playback pb;

    {
      std::lock_guard<std::mutex> lk(ctx.play_mtx);
      pb = ctx.play;
    }

    // Check if we should be playing
    // If not, sleep a bit and check again
    if (!pb.playing || ctx.scenarios.empty() || pb.scenario_index >= ctx.scenarios.size()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(20));
      continue;
    }

    // Playing a scenario, get it
    const Scenario& sc = ctx.scenarios[pb.scenario_index];

    const uint64_t now = now_ms();
    const uint32_t elapsed = static_cast<uint32_t>(now - pb.start_ms);

    // Apply due events to ctx.state (under lock)
    bool reached_end = false;
    {
      std::lock_guard<std::mutex> lk_state(ctx.state_mtx);

      size_t i = pb.next_event_idx;
      while (i < sc.events.size() && sc.events[i].t_ms <= elapsed) {
        for (const auto& kv : sc.events[i].kv) apply_kv(ctx.state, kv.first, kv.second);
        i++;
      }
      reached_end = (i >= sc.events.size());
      // write back next_event_idx
      {
        std::lock_guard<std::mutex> lk_play(ctx.play_mtx);
        // scenario might have changed; only write if still same one and playing
        if (ctx.play.playing && ctx.play.scenario_index == pb.scenario_index) {
          ctx.play.next_event_idx = i;
        }
      }
    }

    // handle end of scenario
    if (reached_end) {
      const uint32_t GRACE_MS = 1000;
      if (elapsed >= sc.duration_ms + GRACE_MS) {
        std::lock_guard<std::mutex> lk(ctx.play_mtx);
        if (ctx.play.playing && ctx.play.scenario_index == pb.scenario_index) {
          if (ctx.play.loop) {
            ctx.play.start_ms = now_ms();
            ctx.play.next_event_idx = 0;
            // reset state to defaults + init
            std::lock_guard<std::mutex> lk_state(ctx.state_mtx);
            apply_scenario_start_locked(ctx, sc);
          } else {
            ctx.play.playing = false;
          }
        }
      }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
}

// -----------------------------
// CAN sender thread
// -----------------------------

static void can_sender_loop(int sock, AppContext& ctx)
{
  uint64_t t0 = now_ms();

  uint64_t next_motor = t0;
  uint64_t next_imu   = t0;
  uint64_t next_mag   = t0;
  uint64_t next_wheel = t0;
  uint64_t next_tof   = t0;
  uint64_t next_env   = t0;
  uint64_t next_batt  = t0;
  uint64_t next_hb    = t0;
  uint64_t next_estop = t0;

  uint8_t motor_counter = 0;

  while (ctx.running.load()) {
    const uint64_t now = now_ms();
    const uint32_t uptime = static_cast<uint32_t>(now - t0);

    // snapshot the latest state (under lock)
    SimState st;
    {
      std::lock_guard<std::mutex> lk(ctx.play_mtx);
      st = ctx.state;
    }
    // snapshot the periods (no lock needed since they are const)
    const Periods& P = ctx.periods;

    // Send frames - check each one
    // if the time passed is equal or greater than the scheduled
    if (now >= next_motor) {
      MotorStatus_t m = make_motor_status(st, motor_counter++);
      send_payload8(sock, CAN_ID_MOTOR_STATUS, &m);
      next_motor += P.motor;
    }

    if (now >= next_imu) {
      ImuAccel_t a = make_accel(st);
      ImuGyro_t  g = make_gyro(st);
      send_payload8(sock, CAN_ID_IMU_ACCEL, &a);
      send_payload8(sock, CAN_ID_IMU_GYRO,  &g);
      next_imu += P.imu_fast;
    }

    if (now >= next_mag) {
      ImuMag_t m = make_mag(st);
      send_payload8(sock, CAN_ID_IMU_MAG, &m);
      next_mag += P.imu_mag;
    }

    if (now >= next_wheel) {
      WheelSpeed_t w = make_wheel(st);
      send_payload8(sock, CAN_ID_WHEEL_SPEED, &w);
      next_wheel += P.wheel;
    }

    if (now >= next_tof) {
      ToFDistance_t t = make_tof(st);
      send_payload8(sock, CAN_ID_TOF_DISTANCE, &t);
      next_tof += P.tof;
    }

    if (now >= next_env) {
      Environment_t e = make_env(st);
      send_payload8(sock, CAN_ID_ENVIRONMENT, &e);
      next_env += P.env;
    }

    if (now >= next_batt) {
      BatteryStatus_t b = make_batt(st);
      send_payload8(sock, CAN_ID_BATTERY, &b);
      next_batt += P.batt;
    }

    if (now >= next_hb) {
      Heartbeat_t h = make_hb(st, uptime);
      send_payload8(sock, CAN_ID_HEARTBEAT_STM32, &h);
      next_hb += P.hb;
    }

    if (now >= next_estop) {
      EmergencyStop_t e = make_estop(st);
      send_payload8(sock, CAN_ID_EMERGENCY_STOP, &e);
      next_estop += P.estop;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(2));
  }
}

// -----------------------------
// CLI
// -----------------------------
static void print_help()
{
  std::cout <<
    "Commands:\n"
    "  help\n"
    "  list\n"
    "  play <index|name>\n"
    "  stop\n"
    "  loop on|off\n"
    "  status\n"
    "  show\n"
    "  reset\n"
    "  reload\n"
    "  tx on|off\n"
    "  quit\n";
}

static void cmd_list(AppContext& ctx)
{
  if (ctx.scenarios.empty()) {
    std::cout << "(no scenarios loaded)\n";
    return;
  }
  for (size_t i = 0; i < ctx.scenarios.size(); ++i) {
    std::cout << i << ") " << ctx.scenarios[i].name
              << "  (events=" << ctx.scenarios[i].events.size()
              << ", duration=" << ctx.scenarios[i].duration_ms << "ms)\n";
  }
}

static bool parse_index_or_name(AppContext& ctx, const std::string& s, size_t& out_idx)
{
  // try integer
  char* end = nullptr;
  long v = std::strtol(s.c_str(), &end, 10);
  if (end && *end == '\0') {
    if (v < 0) return false;
    out_idx = static_cast<size_t>(v);
    return true;
  }

  // try name match
  for (size_t i = 0; i < ctx.scenarios.size(); ++i) {
    if (ctx.scenarios[i].name == s) { out_idx = i; return true; }
  }
  return false;
}

static void cmd_play(const std::string& arg, AppContext& ctx)
{
  if (ctx.scenarios.empty()) {
    std::cout << "No scenarios loaded. Use 'reload' or check the folder.\n";
    return;
  }

  size_t idx = 0;
  if (!parse_index_or_name(ctx, arg, idx) || idx >= ctx.scenarios.size()) {
    std::cout << "Unknown scenario: " << arg << "\n";
    return;
  }

  const Scenario& sc = ctx.scenarios[idx];

  {
    std::lock_guard<std::mutex> lk_state(ctx.state_mtx);
    apply_scenario_start_locked(ctx, sc);
  }

  {
    std::lock_guard<std::mutex> lk(ctx.play_mtx);
    ctx.play.playing = true;
    ctx.play.scenario_index = idx;
    ctx.play.next_event_idx = 0;
    ctx.play.start_ms = now_ms();
  }

  std::cout << "[OK] Playing " << sc.name << " (loop=" << (ctx.play.loop ? "on" : "off") << ")\n";
}

static void cmd_stop(AppContext& ctx)
{
  std::lock_guard<std::mutex> lk(ctx.play_mtx);
  ctx.play.playing = false;
  std::cout << "[OK] Stopped\n";
}

static void cmd_loop(const std::string& arg, AppContext& ctx)
{
  bool on = (arg == "on" || arg == "1" || arg == "true");
  {
    std::lock_guard<std::mutex> lk(ctx.play_mtx);
    ctx.play.loop = on;
  }
  std::cout << "[OK] loop=" << (on ? "on" : "off") << "\n";
}

static void cmd_status(AppContext& ctx)
{
  Playback pb;
  {
    std::lock_guard<std::mutex> lk(ctx.play_mtx);
    pb = ctx.play;
  }

  std::cout << "playing=" << (pb.playing ? "yes" : "no")
            << " loop=" << (pb.loop ? "on" : "off");

  if (pb.playing && !ctx.scenarios.empty() && pb.scenario_index < ctx.scenarios.size()) {
    std::cout << " scenario=" << ctx.scenarios[pb.scenario_index].name
              << " next_event_idx=" << pb.next_event_idx;
  }
  std::cout << "\n";
}

static void cmd_show(AppContext& ctx)
{
  SimState st;
  {
    std::lock_guard<std::mutex> lk(ctx.state_mtx);
    st = ctx.state;
  }

  std::cout
    << "hb_state=" << (int)st.hb_state
    << " hb_errors=" << (int)st.hb_errors
    << " hb_mode=" << (int)st.hb_mode
    << " | estop_active=" << (int)st.estop_active
    << " source=" << (int)st.estop_source
    << " dist_mm=" << st.estop_mm
    << " reason=" << (int)st.estop_reason
    << " | rpm=" << st.rpm
    << " pulses=" << st.pulses
    << " dir=" << (int)st.direction
    << " | tof_mm=" << st.tof_mm
    << " zone=" << (int)st.tof_zone
    << " targets=" << (int)st.tof_count
    << " | env=" << st.env_temp_c << "C " << st.env_humidity << "% " << st.env_light_lux << "lux " << st.env_pressure_hpa << "hPa"
    << " | batt_mv=" << st.batt_mv << " soc=" << (int)st.batt_soc
    << " | motor thr=" << (int)st.motor_throttle << " steer=" << (int)st.motor_steering
    << "\n";
}

static void cmd_reset(AppContext& ctx)
{
  {
    std::lock_guard<std::mutex> lk_state(ctx.state_mtx);
    ctx.state = ctx.defaults;
  }
  std::cout << "[OK] State reset to defaults\n";
}

static void cmd_reload(AppContext& ctx)
{
  try {
    ctx.scenarios = load_scenarios_dir(ctx.scenario_dir);
    std::cout << "[OK] Reloaded scenarios (" << ctx.scenarios.size() << ") from " << ctx.scenario_dir << "\n";
  } catch (const std::exception& e) {
    std::cout << "[ERR] " << e.what() << "\n";
  }
}

static void cli_loop(AppContext& ctx)
{
  print_help();
  std::string line;

  // Main CLI loop
  while (ctx.running.load()) {
    // Prompt
    std::cout << "emu> " << std::flush;
    // Read line
    if (!std::getline(std::cin, line)) {
      ctx.running.store(false);
      break;
    }

    // Trim and skip empty
    line = trim(line);
    if (line.empty()) continue;

    // Parse command
    std::istringstream iss(line);
    std::string cmd;
    iss >> cmd;

    // Handle commands
    if (cmd == "help") print_help();
    else if (cmd == "list") cmd_list(ctx);
    else if (cmd == "play") {
      std::string arg; iss >> arg;
      if (arg.empty()) {
        std::cout << "usage: play <index|name>\n";
      }
      else cmd_play(arg, ctx);
    }
    else if (cmd == "stop") cmd_stop(ctx);
    else if (cmd == "loop") {
      std::string arg; iss >> arg;
      if (arg.empty()) std::cout << "usage: loop on|off\n";
      else cmd_loop(arg, ctx);
    }
    else if (cmd == "status") cmd_status(ctx);
    else if (cmd == "show") cmd_show(ctx);
    else if (cmd == "reset") cmd_reset(ctx);
    else if (cmd == "reload") cmd_reload(ctx);
    else if (cmd == "quit" || cmd == "exit") {
      ctx.running.store(false);
      break;
    }
    else {
      std::cout << "Unknown command. Type 'help'.\n";
    }
  }
}

static void usage(const char* prog)
{
  std::cerr
    << "Usage:\n"
    << "  " << prog << " <iface> <scenario_dir> [--p-wheel=MS] [--p-tof=MS] [--p-imu=MS] [--p-mag=MS] [--p-env=MS] [--p-batt=MS] [--p-hb=MS] [--p-motor=MS] [--p-estop=MS]\n";
}

int main(int argc, char** argv)
{
  if (argc != 1) {
    usage(argv[0]);
    return 1;
  }

  const std::string iface = "vcan0"; // default interface

  AppContext ctx; // create context to hold shared state and config

  // hardcoded scenario directory
  ctx.scenario_dir = "./scenarios";

  // load scenarios (fail if folder missing)
  try {
    ctx.scenarios = load_scenarios_dir(ctx.scenario_dir);
  } catch (const std::exception& e) {
    std::cerr << "[EMU] " << e.what() << "\n";
    return 1;
  }
  if (ctx.scenarios.empty()) {
    std::cerr << "[EMU] No scenarios found in: " << ctx.scenario_dir << "\n";
    return 1;
  }

  std::cout << "[EMU] Loaded " << ctx.scenarios.size()
            << " scenario(s) from " << ctx.scenario_dir << "\n";

  std::cout << "[EMU] IDs (from can_id.h): wheel=0x" << std::hex << CAN_ID_WHEEL_SPEED
            << " imu_acc=0x" << CAN_ID_IMU_ACCEL
            << " imu_gyro=0x" << CAN_ID_IMU_GYRO
            << " imu_mag=0x" << CAN_ID_IMU_MAG
            << " env=0x" << CAN_ID_ENVIRONMENT
            << " batt=0x" << CAN_ID_BATTERY
            << " tof=0x" << CAN_ID_TOF_DISTANCE
            << " hb=0x" << CAN_ID_HEARTBEAT_STM32
            << " estop=0x" << CAN_ID_EMERGENCY_STOP
            << " motor_status=0x" << CAN_ID_MOTOR_STATUS
            << std::dec << "\n";

  const int sock = open_can_tx_socket(iface);
  if (sock < 0) return 1;

  // Threads (IMPORTANT: pass ctx by reference using std::ref)
  std::thread t_player(scenario_player_loop, std::ref(ctx));
  std::thread t_sender(can_sender_loop, sock, std::ref(ctx));

  // Main thread = CLI
  cli_loop(ctx);

  // Shutdown
  ctx.running.store(false);
  if (t_player.joinable()) t_player.join();
  if (t_sender.joinable()) t_sender.join();

  ::close(sock);
  std::cout << "[EMU] Bye.\n";
  return 0;
}

/*
int main(int argc, char** argv)
{
  AppContext ctx;
  if (argc < 3) { usage(argv[0]); return 1; } // (required args: iface, scenario_dir)

  ctx.scenario_dir = argv[2]; // give the scenario dir to the CLI

  ctx.iface = argv[1];
  const std::string iface = argv[1];
  g_scenario_dir = argv[2];

  Periods P;
  auto parse_u32 = [](const std::string& s)->uint32_t {
    return static_cast<uint32_t>(std::strtoul(s.c_str(), nullptr, 10));
  };

  try {
    g_scenarios = load_scenarios_dir(g_scenario_dir);
  } catch (const std::exception& e) {
    std::cerr << "[EMU] " << e.what() << "\n";
    return 1;
  }

  std::cout << "[EMU] Loaded " << g_scenarios.size() << " scenario(s) from " << g_scenario_dir << "\n";
  std::cout << "[EMU] IDs (from can_id.h): wheel=0x" << std::hex << CAN_ID_WHEEL_SPEED
            << " imu_acc=0x" << CAN_ID_IMU_ACCEL
            << " imu_gyro=0x" << CAN_ID_IMU_GYRO
            << " imu_mag=0x" << CAN_ID_IMU_MAG
            << " env=0x" << CAN_ID_ENVIRONMENT
            << " batt=0x" << CAN_ID_BATTERY
            << " tof=0x" << CAN_ID_TOF_DISTANCE
            << " hb=0x" << CAN_ID_HEARTBEAT_STM32
            << " estop=0x" << CAN_ID_EMERGENCY_STOP
            << " motor_status=0x" << CAN_ID_MOTOR_STATUS
            << std::dec << "\n";

  const int sock = open_can_tx_socket(iface);
  if (sock < 0) return 1;

  // threads
  std::thread t_player(scenario_player_loop, ctx);
  std::thread t_sender(can_sender_loop, sock, P);

  // main thread = CLI
  cli_loop();

  // shutdown
  g_running.store(false);
  if (t_player.joinable()) t_player.join();
  if (t_sender.joinable()) t_sender.join();

  ::close(sock);
  std::cout << "[EMU] Bye.\n";
  return 0;
}
*/
