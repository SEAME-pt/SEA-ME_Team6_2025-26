#pragma once

#include <algorithm>
#include <cmath>

// ── PID ───────────────────────────────────────────────────────────────────────
class PID {
public:
    PID(float kp, float ki, float kd) : kp_(kp), ki_(ki), kd_(kd) {}

    void reset() { integral_ = 0.0f; prev_error_ = 0.0f; first_ = true; }

    float compute(float error, float dt) {
        if (first_) { first_ = false; prev_error_ = error; return 0.0f; }
        if (dt <= 0.0f) return 0.0f;

        float p    = kp_ * error;
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

// ── LKA Controller ────────────────────────────────────────────────────────────
// Ported from lka_steering_v1_2.py (Ruben)
// NOTE: if deviation is in cm (calibration enabled), tune kp/kd/deadband
struct LKAConfig {
    float kp        = 4.0f;
    float ki        = 0.0f;
    float kd        = 3.0f;
    float ema_alpha = 0.5f;   // smoothing: 0=frozen, 1=no smoothing
    float deadband  = 2.00f;  // deviations below this → 0
    float snap      = 2.0f;   // PID outputs below this → steering=0
    int   max_rate  = 20;     // max steering change per tick
};

class LKAController {
public:
    explicit LKAController(const LKAConfig& cfg)
        : cfg_(cfg), pid_(cfg.kp, cfg.ki, cfg.kd) {}

    void reset() {
        pid_.reset();
        smooth_dev_    = 0.0f;
        last_steering_ = 0;
    }

    // Returns steering in [-100, 100]
    int compute(float deviation, float dt) {
        // EMA smoothing
        smooth_dev_ = cfg_.ema_alpha * deviation + (1.0f - cfg_.ema_alpha) * smooth_dev_;

        // Deadband
        float filtered = (std::fabs(smooth_dev_) < cfg_.deadband) ? 0.0f : smooth_dev_;

        // PID — target centre (0), error = -deviation
        float output = pid_.compute(-filtered, dt);

        // Centre snap
        if (std::fabs(output) < cfg_.snap) output = 0.0f;

        // Clamp
        output = std::max(-100.0f, std::min(100.0f, output));

        // Rate limit
        int steering = static_cast<int>(std::round(output));
        int delta    = steering - last_steering_;
        if (std::abs(delta) > cfg_.max_rate)
            delta = (delta > 0) ? cfg_.max_rate : -cfg_.max_rate;
        steering       = std::max(-100, std::min(100, last_steering_ + delta));
        last_steering_ = steering;
        return steering;
    }

    int last_steering() const { return last_steering_; }

private:
    LKAConfig cfg_;
    PID       pid_;
    float     smooth_dev_    = 0.0f;
    int       last_steering_ = 0;
};
