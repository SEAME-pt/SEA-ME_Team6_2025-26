#include "socket_receiver.hpp"
#include "can_sender.hpp"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <csignal>
#include <cstdio>
#include <mutex>
#include <thread>

static const char* LANE_SOCKET   = "/tmp/adas_lane.sock";
static const char* OBJECT_SOCKET = "/tmp/adas_objects.sock";

static const char* LANE_STATUS_STR[] = {"none", "left", "right", "both"};

// ── State machine thresholds ──────────────────────────────────────────────────
static constexpr int   DEGRADED_THRESHOLD_FRAMES  = 10;    // 200ms
static constexpr int   EMERGENCY_THRESHOLD_MS     = 500;
static constexpr int   RECOVERY_THRESHOLD_FRAMES  = 15;    // 300ms com lanes válidas para sair de EMERGENCY

// ── LKA parameters (ported from lka_steering_v1_2.py) ────────────────────────
// NOTE: if deviation is in cm (calibration enabled), tune KP/KD/DEADBAND
static constexpr float KP             = 3.0f;
static constexpr float KI             = 0.0f;
static constexpr float KD             = 3.0f;
static constexpr float EMA_ALPHA      = 0.5f;   // smoothing: 0=no update, 1=no smooth
static constexpr float DEADBAND       = 0.08f;   // ignore deviations below this
static constexpr float SNAP           = 3.0f;    // PID outputs below this → steering=0
static constexpr int   MAX_RATE       = 20;      // max steering change per tick
static constexpr int   THROTTLE       = 0;       // set > 0 to move

static std::atomic<bool> running{true};
static void on_signal(int) { running = false; }

// ── State machine ─────────────────────────────────────────────────────────────
enum class AdasState { INIT, ACTIVE, DEGRADED, EMERGENCY_STOP };

static const char* state_str(AdasState s) {
    switch (s) {
        case AdasState::INIT:           return "INIT";
        case AdasState::ACTIVE:         return "ACTIVE";
        case AdasState::DEGRADED:       return "DEGRADED";
        case AdasState::EMERGENCY_STOP: return "EMERGENCY_STOP";
    }
    return "?";
}

// ── PID (ported from lka_steering_v1_2.py) ───────────────────────────────────
class PID {
public:
    PID(float kp, float ki, float kd) : kp_(kp), ki_(ki), kd_(kd) {}

    void reset() { integral_ = 0.0f; prev_error_ = 0.0f; first_ = true; }

    float compute(float error, float dt) {
        if (first_) { first_ = false; prev_error_ = error; return 0.0f; }
        if (dt <= 0.0f) return 0.0f;

        float p = kp_ * error;

        integral_ += error * dt;
        integral_  = std::max(-100.0f, std::min(100.0f, integral_));
        float i    = ki_ * integral_;

        float d    = kd_ * (error - prev_error_) / dt;
        prev_error_ = error;

        return p + i + d;
    }

private:
    float kp_, ki_, kd_;
    float integral_   = 0.0f;
    float prev_error_ = 0.0f;
    bool  first_      = true;
};

// ── Shared state ──────────────────────────────────────────────────────────────
struct SharedState {
    std::mutex  mtx;
    LaneFrame   lane{};
    bool        lane_valid   = false;
    ObjectFrame object{};
    bool        object_valid = false;
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

// ── Main ──────────────────────────────────────────────────────────────────────
int main() {
    signal(SIGINT,  on_signal);
    signal(SIGTERM, on_signal);

    SharedState state;
    std::thread t_lane(lane_thread,  std::ref(state));
    std::thread t_obj (object_thread, std::ref(state));

    CanSender can;
    if (can.init() < 0)
        fprintf(stderr, "[CAN] Failed to init — running without CAN output\n");
    else
        printf("[CAN] %s OK\n", CAN_CHANNEL);

    printf("[ADAS] Manager running. Ctrl+C to stop.\n");

    AdasState adas_state      = AdasState::INIT;
    int       degraded_frames = 0;
    int       recovery_frames = 0;
    auto      degraded_since  = std::chrono::steady_clock::now();
    auto      last_tick       = std::chrono::steady_clock::now();

    PID pid(KP, KI, KD);
    float smooth_dev    = 0.0f;
    int   last_steering = 0;

    while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));

        auto  now = std::chrono::steady_clock::now();
        float dt  = std::chrono::duration_cast<std::chrono::microseconds>(now - last_tick).count() / 1e6f;
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

        // lane_status=none (0) counts as no data — triggers DEGRADED
        bool lane_ok = lane_valid && (lane.lane_status != 0);

        // ── State transitions ─────────────────────────────────────────────────
        if (adas_state == AdasState::EMERGENCY_STOP) {
            if (lane_ok) {
                recovery_frames++;
                if (recovery_frames >= RECOVERY_THRESHOLD_FRAMES) {
                    adas_state      = AdasState::ACTIVE;
                    degraded_frames = 0;
                    recovery_frames = 0;
                    pid.reset();
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
                        pid.reset();
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

        // ── LKA control ───────────────────────────────────────────────────────
        int steering = 0;

        switch (adas_state) {
            case AdasState::ACTIVE: {
                float dev = lane.lateral_deviation;

                // EMA smoothing on deviation (removes frame-to-frame noise)
                smooth_dev = EMA_ALPHA * dev + (1.0f - EMA_ALPHA) * smooth_dev;

                // Deadband
                float filtered = (std::fabs(smooth_dev) < DEADBAND) ? 0.0f : smooth_dev;

                // PID — target is centre (0), error = 0 - deviation
                float output = pid.compute(-filtered, dt);

                // Centre snap
                if (std::fabs(output) < SNAP) output = 0.0f;

                // Clamp + rate limit
                output   = std::max(-100.0f, std::min(100.0f, output));
                steering = static_cast<int>(std::round(output));
                int delta = steering - last_steering;
                if (std::abs(delta) > MAX_RATE)
                    delta = (delta > 0) ? MAX_RATE : -MAX_RATE;
                steering = std::max(-100, std::min(100, last_steering + delta));

                can.send_control(static_cast<int16_t>(steering),
                                 static_cast<int16_t>(THROTTLE));
                break;
            }

            case AdasState::DEGRADED:
                // Hold last known steering
                steering = last_steering;
                can.send_control(static_cast<int16_t>(steering),
                                 static_cast<int16_t>(THROTTLE));
                break;

            case AdasState::EMERGENCY_STOP:
                steering = 0;
                can.send_control(0, 0);
                break;

            case AdasState::INIT:
                break;
        }

        last_steering = steering;

        // ── Log ───────────────────────────────────────────────────────────────
        const char* lane_str = (lane.lane_status < 4)
                               ? LANE_STATUS_STR[lane.lane_status] : "?";

        printf("[ADAS][%-14s] ", state_str(adas_state));

        if (lane_valid)
            printf("lane=%-5s  dev=%+.2f  steer=%+4d", lane_str,
                   lane.lateral_deviation, steering);
        else
            printf("lane=---              steer=%+4d", steering);

        if (obj_valid) {
            printf("  | obj=%u sign(s)", obj.count);
            for (uint8_t i = 0; i < obj.count && i < MAX_OBJECTS; ++i)
                printf("  [class=%u conf=%.2f dist=%.2fm]",
                       obj.objects[i].class_id,
                       obj.objects[i].confidence,
                       obj.objects[i].distance);
            printf("\n");
        } else {
            printf("  | obj=---\n");
        }
    }

    can.send_control(0, 0);
    can.close_fd();

    t_lane.join();
    t_obj.join();
    printf("[ADAS] Shutdown.\n");
    return 0;
}
