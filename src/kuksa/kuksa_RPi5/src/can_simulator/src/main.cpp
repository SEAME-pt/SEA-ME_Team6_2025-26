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


#include "../../../inc/can_id.h"
#include "../inc/helpers.hpp"
#include "../inc/sim_state.hpp"
#include "../inc/app_context.hpp"
#include "../inc/can_sender.hpp"
#include "../inc/scenario_loader.hpp"
#include "../inc/frames_builder.hpp"
#include "../inc/cli.hpp"

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