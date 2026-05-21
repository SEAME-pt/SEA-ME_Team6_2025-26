#pragma once

#include <cmath>
#include <algorithm>
#include <cstdint>

struct OAConfig {
    float wheelbase_m      = 0.18f;
    float car_width_m      = 0.18f;
    float margin_m         = 0.03f;
    float critical_dist_m  = 0.60f;
    float servo_max_deg    = 15.0f;
    float throttle_evading = 15.0f;
    int   evade_ms         = 600;   // phase 1: steer to clear obstacle
    int   straight_ms      = 400;   // phase 2: straight past obstacle
    int   return_ms        = 600;   // phase 3: symmetric return (= evade_ms)
};

enum class OAState { NORMAL, EVADING, BLOCKED };
enum class OAPhase { IDLE, EVADE, STRAIGHT, RETURN };

struct OAResult {
    OAState state    = OAState::NORMAL;
    int8_t  steering = 0;
    int8_t  throttle = 0;
};

static const char* oa_state_str(OAState s) {
    switch (s) {
        case OAState::NORMAL:  return "NORMAL ";
        case OAState::EVADING: return "EVADING";
        case OAState::BLOCKED: return "BLOCKED";
    }
    return "?";
}

class OAController {
public:
    explicit OAController(const OAConfig& cfg) : cfg_(cfg) {}

    // dt_ms: elapsed milliseconds since last call (typically 20)
    OAResult step(float d_srf_m, float d_cam_m, float theta_cam_deg,
                  bool cam_valid, int dt_ms = 20) {
        switch (phase_) {
            case OAPhase::IDLE:
                return _idle(d_srf_m, d_cam_m, theta_cam_deg, cam_valid);
            case OAPhase::EVADE:
                return _run_phase(evade_steering_, cfg_.evade_ms,
                                  OAPhase::STRAIGHT, dt_ms);
            case OAPhase::STRAIGHT:
                return _run_phase(0, cfg_.straight_ms,
                                  OAPhase::RETURN, dt_ms);
            case OAPhase::RETURN:
                return _run_phase(-evade_steering_, cfg_.return_ms,
                                  OAPhase::IDLE, dt_ms);
        }
        return { OAState::NORMAL, 0, 0 };
    }

    OAState state() const {
        return (phase_ == OAPhase::IDLE) ? OAState::NORMAL : OAState::EVADING;
    }

    void reset() {
        phase_    = OAPhase::IDLE;
        timer_ms_ = 0;
        evade_steering_ = 0;
    }

private:
    // Called only in IDLE — checks for obstacle and triggers maneuver
    OAResult _idle(float d_srf_m, float d_cam_m, float theta_cam_deg, bool cam_valid) {
        const float clearance = cfg_.car_width_m / 2.0f + cfg_.margin_m;

        bool srf_trigger = (d_srf_m < cfg_.critical_dist_m);

        float x_obj = 0.0f, y_obj = 0.0f;
        bool cam_in_path = false;
        if (cam_valid) {
            float theta_rad = theta_cam_deg * (static_cast<float>(M_PI) / 180.0f);
            x_obj       = d_cam_m * std::cos(theta_rad);
            y_obj       = d_cam_m * std::sin(theta_rad);
            cam_in_path = (d_cam_m < cfg_.critical_dist_m) &&
                          (std::abs(y_obj) < clearance);
        }

        if (!srf_trigger && !cam_in_path)
            return { OAState::NORMAL, 0, 0 };

        // SRF triggered but no YOLO angle — can't compute maneuver
        if (!cam_in_path)
            return { OAState::BLOCKED, 0, 0 };

        // Compute target waypoint: pass obstacle with clearance
        float y_alvo = (y_obj >= 0.0f) ? (y_obj - clearance) : (y_obj + clearance);

        if (std::abs(y_alvo) < 1e-4f)
            return { OAState::NORMAL, 0, static_cast<int8_t>(cfg_.throttle_evading) };

        float R         = (x_obj * x_obj + y_alvo * y_alvo) / (2.0f * std::abs(y_alvo));
        float alpha_deg = std::atan(cfg_.wheelbase_m / R) * (180.0f / static_cast<float>(M_PI));
        if (y_obj >= 0.0f) alpha_deg = -alpha_deg;  // obstacle right → steer left

        float normalised = (alpha_deg / cfg_.servo_max_deg) * 100.0f;

        if (std::abs(normalised) > 100.0f)
            return { OAState::BLOCKED, 0, 0 };

        // Commit to maneuver — store steering, start EVADE phase
        evade_steering_ = static_cast<int8_t>(std::clamp(normalised, -100.0f, 100.0f));
        phase_          = OAPhase::EVADE;
        timer_ms_       = 0;

        return {
            OAState::EVADING,
            evade_steering_,
            static_cast<int8_t>(cfg_.throttle_evading)
        };
    }

    // Advances timer, transitions to next phase when done
    OAResult _run_phase(int8_t steering, int duration_ms, OAPhase next, int dt_ms) {
        timer_ms_ += dt_ms;
        if (timer_ms_ >= duration_ms) {
            timer_ms_ = 0;
            phase_    = next;
            if (next == OAPhase::IDLE)
                return { OAState::NORMAL, 0, 0 };  // maneuver complete — LKA takes over
        }
        return {
            OAState::EVADING,
            steering,
            static_cast<int8_t>(cfg_.throttle_evading)
        };
    }

    OAConfig cfg_;
    OAPhase  phase_          = OAPhase::IDLE;
    int8_t   evade_steering_ = 0;
    int      timer_ms_       = 0;
};
