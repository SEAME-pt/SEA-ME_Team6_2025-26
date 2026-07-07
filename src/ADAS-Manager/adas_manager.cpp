#include "config.hpp"
#include "socket_receiver.hpp"
#include "can_sender.hpp"
#include "lka_controller.hpp"
#include "joystick_receiver.hpp"
#include "oa_controller.hpp"
#include "can_receiver.hpp"
#include "can_id.h"
#include "can_protocol.h"
#include "drive_mode.hpp"
#include "kuksa_bridge.hpp"
#include "adas_state.hpp"
#include "telemetry_log.hpp"
#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdio>
#include <mutex>
#include <thread>

// ── Sockets / Config ──────────────────────────────────────────────────────────
static const char* LANE_SOCKET   = "/tmp/adas_lane.sock";
static const char* OBJECT_SOCKET = "/tmp/adas_objects.sock";
static const char* CONFIG_PATH   = "/data/current/lka_config.conf";
static const char* BRIDGE_CMD    = "python3 /data/current/kuksa_bridge.py";

// ── Signal ────────────────────────────────────────────────────────────────────
static std::atomic<bool> running{true};
static void on_signal(int) { running = false; }

// ── Shared state ──────────────────────────────────────────────────────────────
struct StateSnapshot {
    LaneFrame   lane{};
    bool        lane_valid   = false;
    ObjectFrame object{};
    bool        object_valid = false;
    int8_t      joy_steering = 0;
    int8_t      joy_throttle = 0;
    bool        joy_valid        = false;
    bool        joy_toggle       = false;
    bool        joy_force_manual = false;
    bool        joy_force_auto    = false;
    uint16_t    current_speed_cms = 0;
    uint16_t    gap_cm            = 0xFFFF;
    bool        status_valid      = false;

    std::chrono::steady_clock::time_point last_lane_ts{};
    std::chrono::steady_clock::time_point last_obj_ts{};
    std::chrono::steady_clock::time_point last_joy_ts{};
};

struct SharedState {
    std::mutex  mtx;
    LaneFrame   lane{};
    bool        lane_valid       = false;
    ObjectFrame object{};
    bool        object_valid     = false;
    int8_t      joy_steering     = 0;
    int8_t      joy_throttle     = 0;
    bool        joy_valid        = false;
    bool        joy_toggle       = false;
    bool        joy_force_manual = false;
    bool        joy_force_auto    = false;
    uint16_t    current_speed_cms = 0;
    uint16_t    gap_cm            = 0xFFFF;
    bool        status_valid      = false;

    std::chrono::steady_clock::time_point last_lane_ts{};
    std::chrono::steady_clock::time_point last_obj_ts{};
    std::chrono::steady_clock::time_point last_joy_ts{};

    StateSnapshot snapshot() {
        std::lock_guard<std::mutex> lk(mtx);
        StateSnapshot s;
        s.lane         = lane;
        s.lane_valid   = lane_valid;
        s.last_lane_ts = last_lane_ts;
        s.object       = object;
        s.object_valid = object_valid;
        s.last_obj_ts  = last_obj_ts;
        s.joy_steering = joy_steering;
        s.joy_throttle = joy_throttle;
        s.joy_valid    = joy_valid;
        s.joy_toggle       = joy_toggle;
        s.joy_force_manual  = joy_force_manual;
        s.joy_force_auto    = joy_force_auto;
        s.last_joy_ts       = last_joy_ts;
        s.current_speed_cms = current_speed_cms;
        s.gap_cm            = gap_cm;
        s.status_valid      = status_valid;
        joy_toggle          = false;
        joy_force_manual    = false;
        joy_force_auto      = false;
        return s;
    }
};

// ── Receiver threads ──────────────────────────────────────────────────────────
void status_thread(SharedState& state) {
    CanReceiver rx;
    if (rx.init() < 0) {
        fprintf(stderr, "[STATUS] Failed to init CAN receiver — speed feedback disabled\n");
        return;
    }
    printf("[STATUS] Listening on CAN (0x213)\n");

    while (running) {
        CtrlStatus_t st{};
        if (rx.read_ctrl_status(st)) {
            std::lock_guard<std::mutex> lk(state.mtx);
            state.current_speed_cms = st.current_speed_cms;
            state.gap_cm            = st.gap_cm;
            state.status_valid      = true;
        }
    }
    rx.close_fd();
}

void lane_thread(SharedState& state) {
    SocketReceiver rx(LANE_SOCKET);
    if (rx.init() < 0) { fprintf(stderr, "[LANE] Failed to init socket\n"); return; }
    printf("[LANE] Listening on %s\n", LANE_SOCKET);

    LaneFrame frame{};
    while (running) {
        bool ok = rx.receiveLatest(frame);
        std::lock_guard<std::mutex> lk(state.mtx);
        state.lane_valid = ok;
        if (ok) {
            state.lane        = frame;
            state.last_lane_ts = std::chrono::steady_clock::now();
        }
    }
    rx.close_fd();
}

void object_thread(SharedState& state) {
    SocketReceiver rx(OBJECT_SOCKET);
    if (rx.init() < 0) { fprintf(stderr, "[OBJ] Failed to init socket\n"); return; }
    printf("[OBJ] Listening on %s\n", OBJECT_SOCKET);

    ObjectFrame frame{};
    while (running) {
        bool ok = rx.receiveLatest(frame);
        std::lock_guard<std::mutex> lk(state.mtx);
        state.object_valid = ok;
        if (ok) {
            state.object      = frame;
            state.last_obj_ts  = std::chrono::steady_clock::now();
        }
    }
    rx.close_fd();
}

void joystick_thread(SharedState& state) {
    static constexpr int RETRY_INTERVAL_MS = 1000;

    JoystickReceiver rx;

    // Retry loop — recovers from transient init failures without killing the thread
    while (running) {
        if (rx.init() >= 0) break;
        fprintf(stderr, "[JOY] Failed to init %s — retry in %dms\n",
                JOYSTICK_SOCKET, RETRY_INTERVAL_MS);
        std::this_thread::sleep_for(std::chrono::milliseconds(RETRY_INTERVAL_MS));
    }

    if (!running) return;
    printf("[JOY] Listening on %s\n", JOYSTICK_SOCKET);

    while (running) {
        JoystickMsg msg = rx.receive();
        std::lock_guard<std::mutex> lk(state.mtx);
        if (msg.type == JoystickMsg::Type::J) {
            state.joy_steering = msg.steering;
            state.joy_throttle = msg.throttle;
            state.joy_valid    = true;
            state.last_joy_ts  = std::chrono::steady_clock::now();
        } else if (msg.type == JoystickMsg::Type::T) {
            state.joy_toggle = true;
        } else if (msg.type == JoystickMsg::Type::FORCE_MANUAL) {
            state.joy_force_manual = true;
        } else if (msg.type == JoystickMsg::Type::FORCE_AUTO) {
            state.joy_force_auto = true;
        }
    }
    rx.close_fd();
}

// ── Object throttle override ──────────────────────────────────────────────────
static int obj_throttle_limit(const ObjectFrame& obj, bool obj_valid,
                               float conf_thresh, float collision_dist_m) {
    if (!obj_valid || obj.count == 0) return 100;

    for (uint8_t i = 0; i < obj.count && i < MAX_OBJECTS; ++i) {
        const auto& o = obj.objects[i];
        if (o.confidence < conf_thresh) continue;
        if (o.distance < collision_dist_m) return 0;
        if (o.class_id == SIGN_STOP   || o.class_id == SIGN_TL_RED)    return 0;
        if (o.class_id == SIGN_YIELD  || o.class_id == SIGN_TL_YELLOW) return 50;
    }
    return 100;
}

// ── ADAS state machine (AUTONOMOUS only) ─────────────────────────────────────
static void adas_state_machine(
    bool lane_ok,
    std::chrono::steady_clock::time_point now,
    const AdasConfig& cfg,
    AdasState& adas_state,
    int& degraded_frames,
    int& recovery_frames,
    std::chrono::steady_clock::time_point& degraded_since,
    std::chrono::steady_clock::time_point& emergency_since,
    bool& estop_sent,
    LKAController& lka,
    CanSender& can)
{
    if (adas_state == AdasState::EMERGENCY_STOP) {
        if (lane_ok) {
            recovery_frames++;
            if (recovery_frames >= cfg.recovery_threshold_frames) {
                adas_state      = AdasState::ACTIVE;
                degraded_frames = 0;
                recovery_frames = 0;
                lka.reset();
                if (estop_sent) {
                    can.send_estop(0);
                    estop_sent = false;
                }
                printf("[ADAS] → ACTIVE (recovered)\n");
            }
        } else {
            recovery_frames = 0;
        }

        /* Timeout-based recovery — deadlock breaker.
         *
         * Cenários reais que isto resolve:
         *   1) Carro pára via STM32 AEB (SRF08) numa posição onde a câmara
         *      já não vê lane suficiente. lane_ok fica falso indefinidamente,
         *      recovery normal nunca dispara.
         *   2) Câmara é tapada momentaneamente → EMERGENCY_STOP. Ao destapar,
         *      o detector de lane pode demorar a estabilizar; entretanto o
         *      Manager continua a enviar aeb_request=true em cada tick,
         *      bloqueando o STM32.
         *
         * Solução: ao fim de emergency_timeout_ms em EMERGENCY_STOP, força
         * regresso a DEGRADED (não ACTIVE, por segurança). Em DEGRADED:
         *   - autonomous_driving NÃO envia aeb_request → STM32 fica livre
         *   - se lane voltar a aparecer → transita normal para ACTIVE
         *   - se lane continuar perdida → re-entra em EMERGENCY_STOP após
         *     emergency_threshold_ms (timer recomeça com degraded_since=now)
         *
         * IMPORTANTE: só aplicar se ainda estamos em EMERGENCY_STOP — se
         * o bloco anterior já fez recovery via lane_ok, não sobrescrever.
         */
        if (adas_state == AdasState::EMERGENCY_STOP) {
            auto emergency_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                now - emergency_since).count();
            if (emergency_ms >= cfg.emergency_timeout_ms) {
                adas_state      = AdasState::DEGRADED;
                degraded_since  = now;
                recovery_frames = 0;
                if (estop_sent) {
                    can.send_estop(0);
                    estop_sent = false;
                }
                printf("[ADAS] EMERGENCY_STOP timeout %lds → DEGRADED"
                       " (lane=%s — recovery via timeout)\n",
                       (long)(cfg.emergency_timeout_ms / 1000),
                       lane_ok ? "OK" : "lost");
            }
        }
    } else {
        if (lane_ok) {
            degraded_frames = 0;
            recovery_frames = 0;
            adas_state      = AdasState::ACTIVE;
        } else {
            degraded_frames++;
            if (degraded_frames >= cfg.degraded_threshold_frames) {
                if (adas_state != AdasState::DEGRADED) {
                    adas_state     = AdasState::DEGRADED;
                    degraded_since = now;
                    lka.reset();
                    printf("[ADAS] → DEGRADED\n");
                }
                auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                    now - degraded_since).count();
                if (ms >= cfg.emergency_threshold_ms) {
                    adas_state      = AdasState::EMERGENCY_STOP;
                    emergency_since = now;
                    recovery_frames = 0;
                    printf("[ADAS] → EMERGENCY_STOP\n");
                    if (!estop_sent) {
                        can.send_estop(1, 0x10);
                        estop_sent = true;
                    }
                }
            }
        }
    }
}

// ── Drive mode toggle ─────────────────────────────────────────────────────────
static void change_drive_mode(
    DriveMode& drive_mode,
    AdasState& adas_state,
    int& degraded_frames,
    int& recovery_frames,
    LKAController& lka,
    OAController& oa,
    KuksaBridge& bridge)
{
    drive_mode = (drive_mode == DriveMode::MANUAL)
                 ? DriveMode::AUTONOMOUS : DriveMode::MANUAL;

    adas_state      = AdasState::INIT;
    degraded_frames = 0;
    recovery_frames = 0;
    oa.reset();
    if (drive_mode == DriveMode::AUTONOMOUS)
        lka.reset();

    printf("[ADAS] Drive mode → %s\n",
           drive_mode == DriveMode::MANUAL ? "MANUAL" : "AUTONOMOUS");
    bridge.pub_mode(drive_mode);
}

// ── Drive output ──────────────────────────────────────────────────────────────
struct DriveOutput {
    int      steering         = 0;
    int      throttle         = 0;
    int      throttle_limit   = 0;
    uint16_t target_speed_cms = 0;
    bool     acc_active       = false;
};

// ── Manual driving ────────────────────────────────────────────────────────────
static DriveOutput manual_driving(
    bool joy_valid,
    int8_t joy_steering,
    int8_t joy_throttle,
    int default_throttle,
    bool& estop_sent,
    CanSender& can)
{
    DriveOutput out;
    out.throttle_limit = default_throttle;
    if (joy_valid) {
        out.steering = joy_steering;
        out.throttle = joy_throttle;
        can.send_ctrl_cmd(CTRL_MODE_MANUAL,
                          static_cast<int8_t>(out.steering),
                          static_cast<int8_t>(out.throttle));
    } else {
        can.send_ctrl_cmd(CTRL_MODE_DISABLED, 0, 0);
    }
    if (estop_sent) {
        can.send_estop(0);
        estop_sent = false;
    }
    return out;
}

// ── Autonomous driving ────────────────────────────────────────────────────────
static DriveOutput autonomous_driving(
    AdasState adas_state,
    const LaneFrame& lane,
    const ObjectFrame& obj,
    bool obj_valid,
    float dt,
    const AdasConfig& cfg,
    LKAController& lka,
    OAController& oa,
    bool& estop_sent,
    CanSender& can,
    uint16_t status_gap_cm = 0xFFFF)
{
    DriveOutput out;

    float nearest_dist_m = 9999.0f;
    float nearest_theta  = 0.0f;
    bool  cam_valid_oa   = false;
    if (obj_valid) {
        for (uint8_t i = 0; i < obj.count && i < MAX_OBJECTS; ++i) {
            if (obj.objects[i].class_id == SIGN_OBSTACLE &&
                obj.objects[i].distance < nearest_dist_m) {
                nearest_dist_m = obj.objects[i].distance;
                nearest_theta  = obj.objects[i].theta_cam;
                cam_valid_oa   = true;
            }
        }
    }
    // SRF08 gap from CtrlStatus supplements camera — dead-ahead, no theta
    if (status_gap_cm != 0xFFFF) {
        float gap_m = status_gap_cm / 100.0f;
        if (gap_m < nearest_dist_m) {
            nearest_dist_m = gap_m;
            nearest_theta  = 0.0f;
            cam_valid_oa   = true;
        }
    }

    out.throttle_limit = obj_throttle_limit(obj, obj_valid,
                                            cfg.obj_conf_thresh, cfg.collision_dist_m);
    out.throttle = std::min(cfg.throttle, out.throttle_limit);

    // Run OA first — freeze LKA integrator during maneuver
    static bool prev_oa_active = false;
    OAResult oa_res;
    bool oa_active = false;
    if (cfg.oa_enabled && (adas_state == AdasState::ACTIVE ||
                           adas_state == AdasState::DEGRADED)) {
        int dt_ms = static_cast<int>(dt * 1000.0f);
        oa.adapt_timings(static_cast<float>(cfg.throttle));
        oa_res    = oa.step(9999.0f, nearest_dist_m, nearest_theta,
                            cam_valid_oa, dt_ms);
        oa_active = (oa_res.state == OAState::EVADING);
        if (prev_oa_active && !oa_active)
            lka.reset();  // OA just finished — clear wound-up integral
    }
    prev_oa_active = oa_active;

    bool do_send = false;
    switch (adas_state) {
        case AdasState::ACTIVE:
            out.steering = oa_active ? lka.last_steering()
                                     : lka.compute(lane.lateral_deviation, dt);
            do_send      = true;
            if (estop_sent) { can.send_estop(0); estop_sent = false; }
            break;

        case AdasState::DEGRADED:
            out.steering = lka.last_steering();
            do_send      = true;
            break;

        case AdasState::EMERGENCY_STOP:
            can.send_ctrl_cmd(CTRL_MODE_DISABLED, 0, 0,
                              0, HEADWAY_MEDIUM, /*aeb_request=*/true);
            break;

        case AdasState::INIT:
            break;
    }

    // ── Curve slowdown — computed after LKA steering is known ────────────────
    static float curve_ema = 1.0f;
    if (do_send && adas_state == AdasState::ACTIVE && !oa_active) {
        float steer_norm = std::fabs(static_cast<float>(out.steering)) / 100.0f;
        float raw = std::max(cfg.curve_min_factor, 1.0f - cfg.curve_gain * steer_norm);
        curve_ema = cfg.curve_ema_alpha * raw + (1.0f - cfg.curve_ema_alpha) * curve_ema;
    } else if (adas_state == AdasState::INIT || adas_state == AdasState::EMERGENCY_STOP) {
        curve_ema = 1.0f;
    }
    bool  single_line  = (lane.lane_status == 1 || lane.lane_status == 2);
    float line_factor  = single_line ? cfg.single_line_factor : 1.0f;
    float speed_factor = std::min(curve_ema, line_factor);  // min not multiply — avoid stacking both on curves

    if (do_send) {
        if (cfg.oa_enabled) {
            if (oa_active) {
                // OA overrides — fixed throttle for maneuver timing, no curve slowdown
                out.steering = oa_res.steering;
                out.throttle = oa_res.throttle;
                can.send_ctrl_cmd(CTRL_MODE_LKA,
                                  static_cast<int8_t>(out.steering),
                                  static_cast<int8_t>(out.throttle));
                do_send = false;
            } else if (oa_res.state == OAState::BLOCKED) {
                can.send_ctrl_cmd(CTRL_MODE_DISABLED, 0, 0,
                                  0, HEADWAY_MEDIUM, /*aeb_request=*/true);
                do_send = false;
            }
        }
        if (do_send) {
            float limit_factor = static_cast<float>(out.throttle_limit) / 100.0f;
            if (cfg.acc_enabled && adas_state == AdasState::ACTIVE) {
                out.target_speed_cms = static_cast<uint16_t>(
                    cfg.acc_target_kmh * (100.0f / 3.6f) * speed_factor * limit_factor);
                out.acc_active = true;
                can.send_ctrl_cmd(CTRL_MODE_ACC,
                                  static_cast<int8_t>(out.steering),
                                  0,
                                  out.target_speed_cms,
                                  static_cast<uint8_t>(cfg.acc_headway));
            } else {
                out.throttle = static_cast<int>(
                    std::round(out.throttle * speed_factor));
                out.throttle = std::max(0, std::min(out.throttle, out.throttle_limit));
                can.send_ctrl_cmd(CTRL_MODE_LKA,
                                  static_cast<int8_t>(out.steering),
                                  static_cast<int8_t>(out.throttle));
            }
        }
    }

    return out;
}

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
        bool               lane_valid   = snap.lane_valid;
        bool               obj_valid    = snap.object_valid;
        bool               joy_valid    = snap.joy_valid;
        const auto&        lane_ts      = snap.last_lane_ts;
        const auto&        obj_ts       = snap.last_obj_ts;
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
            : autonomous_driving(adas_state, lane, obj, obj_valid, dt,
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
                 obj_valid, obj, steering, throttle, throttle_limit, cfg.throttle,
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
    t_joy.join();
    t_status.join();
    printf("[ADAS] Shutdown.\n");
    return 0;
}
