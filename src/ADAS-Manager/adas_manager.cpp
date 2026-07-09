#include "config.hpp"
#include "socket_receiver.hpp"
#include "can_sender.hpp"
#include "lka_controller.hpp"
#include "oa_controller.hpp"
#include "can_id.h"
#include "can_protocol.h"
#include "drive_mode.hpp"
#include "kuksa_bridge.hpp"
#include "adas_state.hpp"
#include "telemetry_log.hpp"
#include "shared_state.hpp"
#include "receiver_threads.hpp"
#include "adas_state_machine.hpp"
#include "drive_control.hpp"
#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdio>
#include <thread>

// ── Sockets / Config ──────────────────────────────────────────────────────────
static const char* CONFIG_PATH   = "/data/current/lka_config.conf";
static const char* BRIDGE_CMD    = "python3 /data/current/kuksa_bridge.py";

// ── Signal ────────────────────────────────────────────────────────────────────
std::atomic<bool> running{true};
static void on_signal(int) { running = false; }

// ── Main ──────────────────────────────────────────────────────────────────────
int main() {
    signal(SIGINT,  on_signal);
    signal(SIGTERM, on_signal);
    signal(SIGPIPE, SIG_IGN);   // kuksa bridge subprocess can die; ignore broken pipe

    // ── Config ────────────────────────────────────────────────────────────────
    AdasConfig cfg = load_adas_config(CONFIG_PATH);

    // ── KUKSA bridge (thread separada — não bloqueia o loop de controlo) ──────
    KuksaBridge bridge;
    bridge.start(BRIDGE_CMD);

    // ── Receiver threads ──────────────────────────────────────────────────────
    SharedState state;
    std::thread t_lane  (lane_thread,     std::ref(state));
    std::thread t_obj   (object_thread,   std::ref(state));
    std::thread t_v2i   (v2i_thread,      std::ref(state));
    std::thread t_joy   (joystick_thread, std::ref(state));
    std::thread t_status(status_thread,   std::ref(state));

    // ── CAN ───────────────────────────────────────────────────────────────────
    CanSender can;
    if (can.init() < 0)
        fprintf(stderr, "[CAN] Failed to init — running without CAN output\n");
    else
        printf("[CAN] %s OK\n", CAN_CHANNEL);

    LKAController lka(cfg.lka);
    OAController  oa(cfg.oa);

    printf("[ADAS] Manager running. Ctrl+C to stop.\n");

    AdasState adas_state      = AdasState::INIT;
    DriveMode drive_mode      = DriveMode::MANUAL;
    int       degraded_frames = 0;
    int       recovery_frames = 0;
    auto      degraded_since  = std::chrono::steady_clock::now();
    auto      emergency_since = std::chrono::steady_clock::now();
    auto      last_tick       = std::chrono::steady_clock::now();
    bool      lane_was_stale  = true;
    bool      joy_was_stale   = true;
    bool      estop_sent      = false;

    while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));

        auto  now = std::chrono::steady_clock::now();
        float dt  = std::chrono::duration_cast<std::chrono::microseconds>(
                        now - last_tick).count() / 1e6f;
        last_tick = now;

        StateSnapshot      snap         = state.snapshot();
        const LaneFrame&   lane         = snap.lane;
        const ObjectFrame& obj          = snap.object;
        const V2IFrame&    v2i          = snap.v2i;
        bool               lane_valid   = snap.lane_valid;
        bool               obj_valid    = snap.object_valid;
        bool               v2i_valid    = snap.v2i_valid;
        bool               joy_valid    = snap.joy_valid;
        const auto&        lane_ts      = snap.last_lane_ts;
        const auto&        obj_ts       = snap.last_obj_ts;
        const auto&        v2i_ts       = snap.last_v2i_ts;
        const auto&        joy_ts       = snap.last_joy_ts;
        const bool         joy_toggle         = snap.joy_toggle;
        const int8_t       joy_steering       = snap.joy_steering;
        const int8_t       joy_throttle       = snap.joy_throttle;
        const uint16_t     status_gap_cm      = snap.gap_cm;
        const uint16_t     current_speed_cms  = snap.current_speed_cms;
        const bool         status_valid       = snap.status_valid;

        // ── Watchdog: override valid flags if timestamps are stale ────────────
        auto lane_age_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - lane_ts).count();
        auto obj_age_ms  = std::chrono::duration_cast<std::chrono::milliseconds>(now - obj_ts).count();
        auto v2i_age_ms  = std::chrono::duration_cast<std::chrono::milliseconds>(now - v2i_ts).count();
        auto joy_age_ms  = std::chrono::duration_cast<std::chrono::milliseconds>(now - joy_ts).count();

        bool lane_stale = lane_valid && (lane_age_ms > cfg.lane_timeout_ms);
        bool joy_stale  = joy_valid  && (joy_age_ms  > cfg.joy_timeout_ms);

        if (lane_stale) {
            lane_valid = false;
            if (!lane_was_stale)
                fprintf(stderr, "[WDG] lane thread stale (%ldms) — forcing DEGRADED\n",
                        (long)lane_age_ms);
        }
        if (obj_age_ms > cfg.obj_timeout_ms)
            obj_valid = false;
        if (v2i_age_ms > cfg.obj_timeout_ms)
            v2i_valid = false;
        if (joy_stale && drive_mode == DriveMode::MANUAL) {
            joy_valid = false;
            if (!joy_was_stale)
                fprintf(stderr, "[WDG] joystick stale (%ldms) — safe stop\n",
                        (long)joy_age_ms);
        }

        lane_was_stale = lane_stale;
        joy_was_stale  = joy_stale && (drive_mode == DriveMode::MANUAL);

        if (snap.joy_force_manual && drive_mode != DriveMode::MANUAL) {
            drive_mode = DriveMode::MANUAL;
            oa.reset();
            bridge.pub_mode(drive_mode);
            printf("[ADAS] → MANUAL (forced by client)\n");
        } else if (snap.joy_force_auto && drive_mode != DriveMode::AUTONOMOUS) {
            drive_mode      = DriveMode::AUTONOMOUS;
            adas_state      = AdasState::INIT;
            degraded_frames = 0;
            recovery_frames = 0;
            lka.reset();
            oa.reset();
            bridge.pub_mode(drive_mode);
            printf("[ADAS] → AUTONOMOUS (forced by client)\n");
        } else if (joy_toggle) {
            change_drive_mode(drive_mode, adas_state, degraded_frames, recovery_frames,
                              lka, oa, bridge);
        }

        bool lane_ok = lane_valid && (lane.lane_status != 0);

        // During OA maneuver the car intentionally leaves the lane — don't degrade
        bool oa_maneuver_active = (oa.state() != OAState::NORMAL);

        if (drive_mode == DriveMode::AUTONOMOUS)
            adas_state_machine(lane_ok || oa_maneuver_active, now, cfg, adas_state,
                               degraded_frames, recovery_frames, degraded_since,
                               emergency_since, estop_sent, lka, can);

        // ── Drive ─────────────────────────────────────────────────────────────
        DriveOutput drive_out = (drive_mode == DriveMode::MANUAL)
            ? manual_driving(joy_valid, joy_steering, joy_throttle,
                             cfg.throttle, estop_sent, can)
            : autonomous_driving(adas_state, lane, obj, obj_valid, v2i, v2i_valid, dt,
                                 cfg, lka, oa, estop_sent, can, status_gap_cm);

        const int steering       = drive_out.steering;
        const int throttle       = drive_out.throttle;
        const int throttle_limit = drive_out.throttle_limit;

        // ── KUKSA publish (não bloqueia — enfileira na bridge thread) ─────────
        if (lane_valid)
            bridge.pub_lane(lane.lateral_deviation, lane.lane_status);
        if (obj_valid)
            bridge.pub_objects(obj);

        // ── Log ───────────────────────────────────────────────────────────────
        log_tick(adas_state, drive_mode, oa, lane_valid, lane,
                 obj_valid, obj, v2i_valid, v2i, steering, throttle, throttle_limit, cfg.throttle,
                 drive_out.acc_active, drive_out.target_speed_cms,
                 current_speed_cms, status_valid);
    }

    // ── Shutdown ──────────────────────────────────────────────────────────────
    can.send_ctrl_cmd(CTRL_MODE_DISABLED, 0, 0);
    if (estop_sent) can.send_estop(0);
    can.close_fd();
    bridge.stop();
    t_lane.join();
    t_obj.join();
    t_v2i.join();
    t_joy.join();
    t_status.join();
    printf("[ADAS] Shutdown.\n");
    return 0;
}
