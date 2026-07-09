#pragma once

#include "adas_state.hpp"
#include "config.hpp"
#include "socket_receiver.hpp"
#include "lka_controller.hpp"
#include "oa_controller.hpp"
#include "can_sender.hpp"
#include "can_protocol.h"
#include "adas_state_machine.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>

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
    const V2IFrame& v2i,
    bool v2i_valid,
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

    const int obj_limit = obj_throttle_limit(obj, obj_valid,
                                             cfg.obj_conf_thresh, cfg.collision_dist_m);
    const int v2i_limit = v2i_throttle_limit(v2i, v2i_valid);
    out.throttle_limit  = std::min(obj_limit, v2i_limit);
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
