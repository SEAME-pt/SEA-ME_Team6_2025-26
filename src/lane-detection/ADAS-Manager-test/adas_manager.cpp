#include "socket_receiver.hpp"
#include "can_sender.hpp"
#include "lka_controller.hpp"
#include "joystick_receiver.hpp"
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <csignal>
#include <cstdio>
#include <fstream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <unordered_map>

// ── Sockets / Config ──────────────────────────────────────────────────────────
static const char* LANE_SOCKET   = "/tmp/adas_lane.sock";
static const char* OBJECT_SOCKET = "/tmp/adas_objects.sock";
static const char* CONFIG_PATH   = "/data/lka_config.conf";
static const char* BRIDGE_CMD    = "python3 /data/ADAS-Manager-test-v11/kuksa_bridge.py";

static const char* LANE_STATUS_STR[] = {"none", "left", "right", "both"};

// ── Signal ────────────────────────────────────────────────────────────────────
static std::atomic<bool> running{true};
static void on_signal(int) { running = false; }

// ── State machine ─────────────────────────────────────────────────────────────
enum class AdasState { INIT, ACTIVE, DEGRADED, EMERGENCY_STOP };
enum class DriveMode  { MANUAL, AUTONOMOUS };

static const char* state_str(AdasState s) {
    switch (s) {
        case AdasState::INIT:           return "INIT";
        case AdasState::ACTIVE:         return "ACTIVE";
        case AdasState::DEGRADED:       return "DEGRADED";
        case AdasState::EMERGENCY_STOP: return "EMERGENCY_STOP";
    }
    return "?";
}

// ── Config loader ─────────────────────────────────────────────────────────────
static std::unordered_map<std::string, float> load_config(const char* path) {
    std::unordered_map<std::string, float> cfg;
    std::ifstream f(path);
    if (!f.is_open()) {
        printf("[CONFIG] %s not found — using defaults\n", path);
        return cfg;
    }
    std::string line;
    while (std::getline(f, line)) {
        if (line.empty() || line[0] == '#') continue;
        auto eq = line.find('=');
        if (eq == std::string::npos) continue;
        std::string key = line.substr(0, eq);
        try { cfg[key] = std::stof(line.substr(eq + 1)); }
        catch (...) {}
    }
    printf("[CONFIG] Loaded %s (%zu keys)\n", path, cfg.size());
    return cfg;
}

// ── KUKSA Bridge — thread assíncrona ─────────────────────────────────────────
// O loop de controlo chama pub_lane / pub_objects sem bloquear.
// A thread de bridge faz o fprintf/fflush ao pipe do subprocess.
// Queue com max 4 mensagens — se KUKSA estiver lento, descarta (telemetria).
class KuksaBridge {
public:
    bool start(const char* cmd) {
        pipe_ = popen(cmd, "w");
        if (!pipe_) {
            fprintf(stderr, "[KUKSA] Falhou a lançar bridge: %s\n", cmd);
            return false;
        }
        printf("[KUKSA] Bridge iniciado: %s\n", cmd);
        running_ = true;
        thread_  = std::thread(&KuksaBridge::_loop, this);
        return true;
    }

    void pub_mode(DriveMode mode) {
        if (!pipe_) return;
        char buf[32];
        snprintf(buf, sizeof(buf), "D %s\n",
                 mode == DriveMode::MANUAL ? "MANUAL" : "AUTONOMOUS");
        _enqueue(buf);
    }

    void pub_lane(float deviation, uint8_t status_code) {
        if (!pipe_) return;
        const char* status = (status_code < 4) ? LANE_STATUS_STR[status_code] : "none";
        char buf[64];
        snprintf(buf, sizeof(buf), "L %.4f %s\n", deviation, status);
        _enqueue(buf);
    }

    void pub_objects(const ObjectFrame& obj) {
        if (!pipe_) return;

        float speed_limit = 0.0f;
        char  signs_buf[128]  = "[";
        char  extras_buf[128] = "[";
        int   spos = 1, epos = 1;
        bool  first_sign = true, first_extra = true;

        for (uint8_t i = 0; i < obj.count && i < MAX_OBJECTS; ++i) {
            const auto& o = obj.objects[i];
            switch (o.class_id) {
                case SIGN_SPEED_30: speed_limit = 30.0f; break;
                case SIGN_SPEED_50: speed_limit = 50.0f; break;
                case SIGN_STOP:
                    spos += snprintf(signs_buf + spos, sizeof(signs_buf) - spos - 2,
                                     "%s\"stop\"", first_sign ? "" : ",");
                    first_sign = false; break;
                case SIGN_YIELD:
                    spos += snprintf(signs_buf + spos, sizeof(signs_buf) - spos - 2,
                                     "%s\"yield\"", first_sign ? "" : ",");
                    first_sign = false; break;
                default:
                    epos += snprintf(extras_buf + epos, sizeof(extras_buf) - epos - 2,
                                     "%s\"obj:%.2f:%.1fm\"", first_extra ? "" : ",",
                                     o.confidence, o.distance);
                    first_extra = false; break;
            }
        }
        snprintf(signs_buf  + spos, 3, "]");
        snprintf(extras_buf + epos, 3, "]");

        char msg[256];
        snprintf(msg, sizeof(msg), "O %.1f none %s %s\n",
                 speed_limit, signs_buf, extras_buf);
        _enqueue(msg);
    }

    void stop() {
        running_ = false;
        cv_.notify_all();
        if (thread_.joinable()) thread_.join();
        if (pipe_) { pclose(pipe_); pipe_ = nullptr; }
    }

private:
    static constexpr int MAX_QUEUE = 4;

    FILE*                    pipe_    = nullptr;
    std::atomic<bool>        running_ {false};
    std::thread              thread_;
    std::mutex               mtx_;
    std::condition_variable  cv_;
    std::queue<std::string>  queue_;

    void _enqueue(const char* msg) {
        std::lock_guard<std::mutex> lk(mtx_);
        if ((int)queue_.size() >= MAX_QUEUE) queue_.pop();  // descarta mais antigo
        queue_.push(msg);
        cv_.notify_one();
    }

    void _loop() {
        while (running_) {
            std::unique_lock<std::mutex> lk(mtx_);
            cv_.wait_for(lk, std::chrono::milliseconds(100),
                         [&]{ return !queue_.empty() || !running_; });
            while (!queue_.empty()) {
                std::string msg = std::move(queue_.front());
                queue_.pop();
                lk.unlock();
                if (pipe_) { fputs(msg.c_str(), pipe_); fflush(pipe_); }
                lk.lock();
            }
        }
    }
};

// ── Shared state ──────────────────────────────────────────────────────────────
struct SharedState {
    std::mutex  mtx;
    LaneFrame   lane{};
    bool        lane_valid   = false;
    ObjectFrame object{};
    bool        object_valid = false;
    int8_t      joy_steering = 0;
    int8_t      joy_throttle = 0;
    bool        joy_valid    = false;
    bool        joy_toggle   = false;
};

// ── Receiver threads ──────────────────────────────────────────────────────────
void lane_thread(SharedState& state) {
    SocketReceiver rx(LANE_SOCKET);
    if (rx.init() < 0) { fprintf(stderr, "[LANE] Failed to init socket\n"); return; }
    printf("[LANE] Listening on %s\n", LANE_SOCKET);

    LaneFrame frame{};
    while (running) {
        bool ok = rx.receiveLatest(frame);
        std::lock_guard<std::mutex> lk(state.mtx);
        state.lane_valid = ok;
        if (ok) state.lane = frame;
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
        if (ok) state.object = frame;
    }
    rx.close_fd();
}

void joystick_thread(SharedState& state) {
    JoystickReceiver rx;
    if (rx.init() < 0) {
        fprintf(stderr, "[JOY] Failed to init socket %s\n", JOYSTICK_SOCKET);
        return;
    }
    printf("[JOY] Listening on %s\n", JOYSTICK_SOCKET);

    while (running) {
        JoystickMsg msg = rx.receive();
        std::lock_guard<std::mutex> lk(state.mtx);
        if (msg.type == JoystickMsg::Type::J) {
            state.joy_steering = msg.steering;
            state.joy_throttle = msg.throttle;
            state.joy_valid    = true;
        } else if (msg.type == JoystickMsg::Type::T) {
            state.joy_toggle = true;
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
        if (o.class_id == SIGN_STOP)      return 0;
        if (o.class_id == SIGN_YIELD)     return 50;
    }
    return 100;
}

// ── Main ──────────────────────────────────────────────────────────────────────
int main() {
    signal(SIGINT,  on_signal);
    signal(SIGTERM, on_signal);

    // ── Config ────────────────────────────────────────────────────────────────
    auto cfg = load_config(CONFIG_PATH);
    auto get = [&](const char* k, float def) -> float {
        auto it = cfg.find(k); return it != cfg.end() ? it->second : def;
    };

    LKAConfig lka_cfg;
    lka_cfg.kp        = get("kp",        4.0f);
    lka_cfg.ki        = get("ki",        0.0f);
    lka_cfg.kd        = get("kd",        3.0f);
    lka_cfg.ema_alpha = get("ema_alpha", 0.5f);
    lka_cfg.deadband  = get("deadband",  2.0f);
    lka_cfg.snap      = get("snap",      2.0f);
    lka_cfg.max_rate  = static_cast<int>(get("max_rate", 20.0f));

    const int   THROTTLE                  = static_cast<int>(get("throttle",                   25.0f));
    const int   DEGRADED_THRESHOLD_FRAMES = static_cast<int>(get("degraded_threshold_frames",  10.0f));
    const int   EMERGENCY_THRESHOLD_MS    = static_cast<int>(get("emergency_threshold_ms",    500.0f));
    const int   RECOVERY_THRESHOLD_FRAMES = static_cast<int>(get("recovery_threshold_frames",  15.0f));
    const float OBJ_CONF_THRESH           = get("obj_conf_thresh", 0.60f);
    const float COLLISION_DIST_M          = get("collision_dist_m", 0.30f);

    printf("[CONFIG] kp=%.1f ki=%.1f kd=%.1f deadband=%.1f throttle=%d\n",
           lka_cfg.kp, lka_cfg.ki, lka_cfg.kd, lka_cfg.deadband, THROTTLE);
    printf("[CONFIG] obj_conf_thresh=%.2f collision_dist=%.2fm\n",
           OBJ_CONF_THRESH, COLLISION_DIST_M);

    // ── KUKSA bridge (thread separada — não bloqueia o loop de controlo) ──────
    KuksaBridge bridge;
    bridge.start(BRIDGE_CMD);

    // ── Receiver threads ──────────────────────────────────────────────────────
    SharedState state;
    std::thread t_lane(lane_thread,     std::ref(state));
    std::thread t_obj (object_thread,   std::ref(state));
    std::thread t_joy (joystick_thread, std::ref(state));

    // ── CAN ───────────────────────────────────────────────────────────────────
    CanSender can;
    if (can.init() < 0)
        fprintf(stderr, "[CAN] Failed to init — running without CAN output\n");
    else
        printf("[CAN] %s OK\n", CAN_CHANNEL);

    LKAController lka(lka_cfg);
    printf("[ADAS] Manager running. Ctrl+C to stop.\n");

    AdasState adas_state      = AdasState::INIT;
    DriveMode drive_mode      = DriveMode::MANUAL;
    int       degraded_frames = 0;
    int       recovery_frames = 0;
    auto      degraded_since  = std::chrono::steady_clock::now();
    auto      last_tick       = std::chrono::steady_clock::now();

    while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));

        auto  now = std::chrono::steady_clock::now();
        float dt  = std::chrono::duration_cast<std::chrono::microseconds>(
                        now - last_tick).count() / 1e6f;
        last_tick = now;

        LaneFrame   lane{};
        ObjectFrame obj{};
        bool lane_valid, obj_valid;
        {
            std::lock_guard<std::mutex> lk(state.mtx);
            lane       = state.lane;
            lane_valid = state.lane_valid;
            obj        = state.object;
            obj_valid  = state.object_valid;
        }

        bool   joy_toggle;
        int8_t joy_steering, joy_throttle;
        bool   joy_valid;
        {
            std::lock_guard<std::mutex> lk(state.mtx);
            joy_toggle   = state.joy_toggle;
            joy_steering = state.joy_steering;
            joy_throttle = state.joy_throttle;
            joy_valid    = state.joy_valid;
            state.joy_toggle = false;
        }

        if (joy_toggle) {
            drive_mode = (drive_mode == DriveMode::MANUAL)
                         ? DriveMode::AUTONOMOUS : DriveMode::MANUAL;
            if (drive_mode == DriveMode::AUTONOMOUS) {
                adas_state      = AdasState::INIT;
                degraded_frames = 0;
                recovery_frames = 0;
                lka.reset();
            }
            printf("[ADAS] Drive mode → %s\n",
                   drive_mode == DriveMode::MANUAL ? "MANUAL" : "AUTONOMOUS");
            bridge.pub_mode(drive_mode);
        }

        bool lane_ok = lane_valid && (lane.lane_status != 0);

        if (drive_mode == DriveMode::AUTONOMOUS) {
            if (adas_state == AdasState::EMERGENCY_STOP) {
                if (lane_ok) {
                    recovery_frames++;
                    if (recovery_frames >= RECOVERY_THRESHOLD_FRAMES) {
                        adas_state      = AdasState::ACTIVE;
                        degraded_frames = 0;
                        recovery_frames = 0;
                        lka.reset();
                        printf("[ADAS] → ACTIVE (recovered)\n");
                    }
                } else {
                    recovery_frames = 0;
                }
            } else {
                if (lane_ok) {
                    degraded_frames = 0;
                    recovery_frames = 0;
                    adas_state      = AdasState::ACTIVE;
                } else {
                    degraded_frames++;
                    if (degraded_frames >= DEGRADED_THRESHOLD_FRAMES) {
                        if (adas_state != AdasState::DEGRADED) {
                            adas_state     = AdasState::DEGRADED;
                            degraded_since = now;
                            lka.reset();
                            printf("[ADAS] → DEGRADED\n");
                        }
                        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                            now - degraded_since).count();
                        if (ms >= EMERGENCY_THRESHOLD_MS) {
                            adas_state      = AdasState::EMERGENCY_STOP;
                            recovery_frames = 0;
                            printf("[ADAS] → EMERGENCY_STOP\n");
                            can.send_control(0, 0);
                        }
                    }
                }
            }
        }

        // ── Drive mode: MANUAL forwards joystick, AUTONOMOUS runs LKA ────────
        int steering      = 0;
        int throttle      = 0;
        int throttle_limit = THROTTLE;

        if (drive_mode == DriveMode::MANUAL) {
            if (joy_valid) {
                steering = joy_steering;
                throttle = joy_throttle;
                can.send_control(static_cast<int16_t>(steering),
                                 static_cast<int16_t>(throttle));
            } else {
                can.send_control(0, 0);
            }
        } else {
            throttle_limit = obj_throttle_limit(obj, obj_valid,
                                                OBJ_CONF_THRESH, COLLISION_DIST_M);
            throttle = std::min(THROTTLE, throttle_limit);

            switch (adas_state) {
                case AdasState::ACTIVE:
                    steering = lka.compute(lane.lateral_deviation, dt);
                    can.send_control(static_cast<int16_t>(steering),
                                     static_cast<int16_t>(throttle));
                    break;

                case AdasState::DEGRADED:
                    steering = lka.last_steering();
                    can.send_control(static_cast<int16_t>(steering),
                                     static_cast<int16_t>(throttle));
                    break;

                case AdasState::EMERGENCY_STOP:
                    can.send_control(0, 0);
                    break;

                case AdasState::INIT:
                    break;
            }
        }

        // ── KUKSA publish (não bloqueia — enfileira na bridge thread) ─────────
        if (lane_valid)
            bridge.pub_lane(lane.lateral_deviation, lane.lane_status);
        if (obj_valid)
            bridge.pub_objects(obj);

        // ── Log ───────────────────────────────────────────────────────────────
        const char* lane_str = (lane.lane_status < 4)
                               ? LANE_STATUS_STR[lane.lane_status] : "?";

        printf("[ADAS][%-14s][%s] ", state_str(adas_state),
               drive_mode == DriveMode::MANUAL ? "MANUAL" : "AUTO  ");

        if (lane_valid)
            printf("lane=%-5s  dev=%+.2f  steer=%+4d  throttle=%3d",
                   lane_str, lane.lateral_deviation, steering, throttle);
        else
            printf("lane=---              steer=%+4d  throttle=%3d",
                   steering, throttle);

        if (obj_valid && obj.count > 0) {
            printf("  | obj=%u", obj.count);
            for (uint8_t i = 0; i < obj.count && i < MAX_OBJECTS; ++i)
                printf("  [cls=%u conf=%.2f dist=%.2fm]",
                       obj.objects[i].class_id,
                       obj.objects[i].confidence,
                       obj.objects[i].distance);
            if (throttle_limit < THROTTLE)
                printf("  *** THROTTLE OVERRIDE=%d ***", throttle_limit);
            printf("\n");
        } else {
            printf("  | obj=---\n");
        }
    }

    // ── Shutdown ──────────────────────────────────────────────────────────────
    can.send_control(0, 0);
    can.close_fd();
    bridge.stop();
    t_lane.join();
    t_obj.join();
    t_joy.join();
    printf("[ADAS] Shutdown.\n");
    return 0;
}
