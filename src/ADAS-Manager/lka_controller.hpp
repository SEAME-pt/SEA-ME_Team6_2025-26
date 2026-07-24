#pragma once

#include <algorithm>
#include <chrono>
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
    int   max_rate  = 20;     // max steering change per frame de perceção (~6-10 Hz)
};

class LKAController {
public:
    explicit LKAController(const LKAConfig& cfg)
        : cfg_(cfg), pid_(cfg.kp, cfg.ki, cfg.kd) {}

    void reset() {
        reset_soft();
        last_steering_ = 0;
    }

    // Reset sem descontinuidade: limpa PID/EMA/frame state mas MANTÉM
    // last_steering_. Para retomar controlo a meio do movimento (entrada em
    // DEGRADED, fim de manobra OA) — zerar o steering fazia as rodas saltar
    // para o centro a meio da saída de curva.
    void reset_soft() {
        pid_.reset();
        smooth_dev_ = 0.0f;
        has_frame_  = false;
    }

    // Returns steering in [-100, 100].
    // Corre o PID apenas quando há frame nova (frame_ts muda), com dt = intervalo
    // real entre frames. A 50 Hz sobre uma deviation congelada entre frames, o
    // termo D calculava kd·Δerro/0.02 em cada frame nova (~8× sobrestimado a
    // ~6-10 fps) — picos de steering que eram o zig-zag em reta. Entre frames
    // devolve o último steering; o loop de 50 Hz continua a alimentar o CAN.
    int compute(float deviation, std::chrono::steady_clock::time_point frame_ts) {
        if (has_frame_ && frame_ts == last_frame_ts_)
            return last_steering_;

        const bool first_frame = !has_frame_;
        float dt = 0.0f;
        if (!first_frame) {
            dt = std::chrono::duration_cast<std::chrono::microseconds>(
                     frame_ts - last_frame_ts_).count() / 1e6f;
            // Gaps longos (recovery, OA) não devem gerar derivadas minúsculas
            // nem integrais gigantes — trata como um passo normal grande.
            dt = std::max(0.05f, std::min(0.5f, dt));
        }
        last_frame_ts_ = frame_ts;
        has_frame_     = true;

        // EMA smoothing (ao ritmo das frames). No 1º frame pós-reset semeia
        // com a leitura real: convergir de 0 atrasava a correção ~3 frames
        // (~0,5 s) ao retomar de DEGRADED — o rate limit já trava o arranque.
        smooth_dev_ = first_frame
            ? deviation
            : cfg_.ema_alpha * deviation + (1.0f - cfg_.ema_alpha) * smooth_dev_;

        // Deadband suave: rampa contínua em vez de degrau. O degrau na borda
        // (erro salta 0→±deadband num tick) criava pico de derivada e
        // ciclo-limite em reta.
        float mag      = std::fabs(smooth_dev_) - cfg_.deadband;
        float filtered = (mag > 0.0f) ? std::copysign(mag, smooth_dev_) : 0.0f;

        // PID — target centre (0), error = -deviation
        float output = pid_.compute(-filtered, dt);

        // 1º frame pós-reset: o PID devolve 0 (priming do prev_error) — não
        // é um comando "vai para o centro". Mantém o steering; a correção
        // a sério vem no frame seguinte, já com dt real.
        if (first_frame) return last_steering_;

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
    bool      has_frame_     = false;
    std::chrono::steady_clock::time_point last_frame_ts_{};
};
