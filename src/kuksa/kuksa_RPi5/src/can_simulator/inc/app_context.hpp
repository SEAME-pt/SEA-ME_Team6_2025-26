#pragma once
#include <atomic>
#include <mutex>
#include <string>
#include <vector>

#include "sim_state.hpp"
#include "../../../inc/can_id.h"

struct Periods {
  uint32_t motor;
  uint32_t imu_fast;
  uint32_t imu_mag;
  uint32_t wheel;
  uint32_t tof;
  uint32_t env;
  uint32_t batt;
  uint32_t hb;
  uint32_t estop;
};

// -----------------------------
// Scenario model
// -----------------------------
struct Event {
  uint32_t t_ms = 0;
  std::vector<std::pair<std::string,std::string>> kv;
};

struct Scenario {
  std::string name;   // filename
  std::string path;   // full path
  std::vector<std::pair<std::string,std::string>> init_kv;
  std::vector<Event> events;
  uint32_t duration_ms = 0;
};

// -----------------------------
// Playback Control State
// -----------------------------
struct Playback {
  bool playing = false;
  bool loop = false;

  size_t scenario_index = 0;
  size_t next_event_idx = 0;

  uint64_t start_ms = 0;
};

struct AppContext {
  // lifecycle
  std::atomic<bool> running{true};

  // config
  std::string scenario_dir;
  Periods periods{
    CAN_PERIOD_MOTOR_STATUS_MS,
    CAN_PERIOD_IMU_FAST_MS,
    CAN_PERIOD_IMU_MAG_MS,
    CAN_PERIOD_WHEEL_SPEED_MS,
    CAN_PERIOD_TOF_MS,
    CAN_PERIOD_ENVIRONMENT_MS,
    CAN_PERIOD_BATTERY_MS,
    CAN_PERIOD_HEARTBEAT_MS,
    CAN_PERIOD_HEARTBEAT_MS
  };

  // scenarios + playback control
  std::mutex play_mtx;
  Playback play{};
  std::vector<Scenario> scenarios;

  // shared simulated state
  std::mutex state_mtx;
  SimState state{};
  SimState defaults{};
};