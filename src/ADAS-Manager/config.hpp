#pragma once
#include "lka_controller.hpp"
#include "oa_controller.hpp"
#include <cstdio>
#include <fstream>
#include <string>
#include <unordered_map>

struct AdasConfig {
    LKAConfig lka;
    OAConfig  oa;

    int   throttle                  = 25;
    int   degraded_threshold_frames = 10;
    int   emergency_threshold_ms    = 500;
    int   recovery_threshold_frames = 15;
    float obj_conf_thresh           = 0.60f;
    float collision_dist_m          = 0.30f;
    int   lane_timeout_ms           = 500;
    int   obj_timeout_ms            = 1000;
    int   joy_timeout_ms            = 200;
    bool  oa_enabled                = true;

    // Adaptive Cruise Control
    bool  acc_enabled      = false;
    float acc_target_kmh   = 3.5f;
    int   acc_headway      = 1;

    // Curve slowdown
    float curve_gain        = 0.9f;
    float curve_min_factor  = 0.6f;
    float curve_ema_alpha   = 0.2f;
    float single_line_factor = 0.6f;
};

static AdasConfig load_adas_config(const char* path) {
    std::unordered_map<std::string, float> raw;
    std::ifstream f(path);
    if (!f.is_open()) {
        printf("[CONFIG] %s not found — using defaults\n", path);
    } else {
        std::string line;
        while (std::getline(f, line)) {
            if (line.empty() || line[0] == '#') continue;
            auto eq = line.find('=');
            if (eq == std::string::npos) continue;
            std::string key = line.substr(0, eq);
            try { raw[key] = std::stof(line.substr(eq + 1)); }
            catch (...) {}
        }
        printf("[CONFIG] Loaded %s (%zu keys)\n", path, raw.size());
    }

    auto get = [&](const char* k, float def) -> float {
        auto it = raw.find(k); return it != raw.end() ? it->second : def;
    };

    AdasConfig cfg;

    cfg.lka.kp        = get("kp",        4.0f);
    cfg.lka.ki        = get("ki",        0.0f);
    cfg.lka.kd        = get("kd",        3.0f);
    cfg.lka.ema_alpha = get("ema_alpha", 0.5f);
    cfg.lka.deadband  = get("deadband",  2.0f);
    cfg.lka.snap      = get("snap",      2.0f);
    cfg.lka.max_rate  = static_cast<int>(get("max_rate", 20.0f));

    cfg.throttle                  = static_cast<int>(get("throttle",                   25.0f));
    cfg.degraded_threshold_frames = static_cast<int>(get("degraded_threshold_frames",  10.0f));
    cfg.emergency_threshold_ms    = static_cast<int>(get("emergency_threshold_ms",    500.0f));
    cfg.recovery_threshold_frames = static_cast<int>(get("recovery_threshold_frames",  15.0f));
    cfg.obj_conf_thresh           = get("obj_conf_thresh",   0.60f);
    cfg.collision_dist_m          = get("collision_dist_m",  0.30f);
    cfg.lane_timeout_ms           = static_cast<int>(get("lane_timeout_ms",  500.0f));
    cfg.obj_timeout_ms            = static_cast<int>(get("obj_timeout_ms",  1000.0f));
    cfg.joy_timeout_ms            = static_cast<int>(get("joy_timeout_ms",   200.0f));
    cfg.oa_enabled                = get("oa_enabled", 1.0f) >= 0.5f;

    cfg.acc_enabled       = get("acc_enabled",     0.0f) >= 0.5f;
    cfg.acc_target_kmh    = get("acc_target_kmh",  3.5f);
    cfg.acc_headway       = static_cast<int>(get("acc_headway", 1.0f));

    cfg.curve_gain         = get("curve_gain",         0.9f);
    cfg.curve_min_factor   = get("curve_min_factor",   0.6f);
    cfg.curve_ema_alpha    = get("curve_ema_alpha",    0.2f);
    cfg.single_line_factor = get("single_line_factor", 0.6f);

    cfg.oa.wheelbase_m      = get("oa_wheelbase_m",      0.18f);
    cfg.oa.car_width_m      = get("oa_car_width_m",      0.18f);
    cfg.oa.margin_m         = get("oa_margin_m",         0.03f);
    cfg.oa.critical_dist_m  = get("oa_critical_dist_m",  0.60f);
    cfg.oa.servo_max_deg    = get("oa_servo_max_deg",    15.0f);
    cfg.oa.throttle_evading = get("oa_throttle_evading", 20.0f);

    // Adaptive timing — lo setpoint
    cfg.oa.throttle_lo    = get("oa_throttle_lo",    20.0f);
    cfg.oa.evade_ms_lo    = static_cast<int>(get("oa_evade_ms_lo",    1700.0f));
    cfg.oa.straight_ms_lo = static_cast<int>(get("oa_straight_ms_lo",  700.0f));
    cfg.oa.return_ms_lo   = static_cast<int>(get("oa_return_ms_lo",   2500.0f));

    // Adaptive timing — hi setpoint
    cfg.oa.throttle_hi    = get("oa_throttle_hi",    28.0f);
    cfg.oa.evade_ms_hi    = static_cast<int>(get("oa_evade_ms_hi",    1000.0f));
    cfg.oa.straight_ms_hi = static_cast<int>(get("oa_straight_ms_hi",  200.0f));
    cfg.oa.return_ms_hi   = static_cast<int>(get("oa_return_ms_hi",   1800.0f));

    // Initialise current timings from lo setpoint
    cfg.oa.evade_ms    = cfg.oa.evade_ms_lo;
    cfg.oa.straight_ms = cfg.oa.straight_ms_lo;
    cfg.oa.return_ms   = cfg.oa.return_ms_lo;

    printf("[CONFIG] kp=%.1f ki=%.1f kd=%.1f deadband=%.1f throttle=%d\n",
           cfg.lka.kp, cfg.lka.ki, cfg.lka.kd, cfg.lka.deadband, cfg.throttle);
    printf("[CONFIG] obj_conf_thresh=%.2f collision_dist=%.2fm\n",
           cfg.obj_conf_thresh, cfg.collision_dist_m);

    return cfg;
}
