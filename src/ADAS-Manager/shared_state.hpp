#pragma once

#include "socket_receiver.hpp"
#include <chrono>
#include <cstdint>
#include <mutex>

// ── Shared state ──────────────────────────────────────────────────────────────
struct StateSnapshot {
    LaneFrame   lane{};
    bool        lane_valid   = false;
    ObjectFrame object{};
    bool        object_valid = false;
    V2IFrame    v2i{};
    bool        v2i_valid    = false;
    int8_t      joy_steering = 0;
    int8_t      joy_throttle = 0;
    bool        joy_valid        = false;
    bool        joy_toggle       = false;
    bool        joy_force_manual = false;
    bool        joy_force_auto    = false;
    bool        joy_emergency_toggle = false;
    uint16_t    current_speed_cms = 0;
    uint16_t    gap_cm            = 0xFFFF;
    bool        status_valid      = false;

    std::chrono::steady_clock::time_point last_lane_ts{};
    std::chrono::steady_clock::time_point last_obj_ts{};
    std::chrono::steady_clock::time_point last_v2i_ts{};
    std::chrono::steady_clock::time_point last_joy_ts{};
};

struct SharedState {
    std::mutex  mtx;
    LaneFrame   lane{};
    bool        lane_valid       = false;
    ObjectFrame object{};
    bool        object_valid     = false;
    V2IFrame    v2i{};
    bool        v2i_valid        = false;
    int8_t      joy_steering     = 0;
    int8_t      joy_throttle     = 0;
    bool        joy_valid        = false;
    bool        joy_toggle       = false;
    bool        joy_force_manual = false;
    bool        joy_force_auto    = false;
    bool        joy_emergency_toggle = false;
    uint16_t    current_speed_cms = 0;
    uint16_t    gap_cm            = 0xFFFF;
    bool        status_valid      = false;

    std::chrono::steady_clock::time_point last_lane_ts{};
    std::chrono::steady_clock::time_point last_obj_ts{};
    std::chrono::steady_clock::time_point last_v2i_ts{};
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
        s.v2i          = v2i;
        s.v2i_valid    = v2i_valid;
        s.last_v2i_ts  = last_v2i_ts;
        s.joy_steering = joy_steering;
        s.joy_throttle = joy_throttle;
        s.joy_valid    = joy_valid;
        s.joy_toggle       = joy_toggle;
        s.joy_force_manual  = joy_force_manual;
        s.joy_force_auto    = joy_force_auto;
        s.joy_emergency_toggle = joy_emergency_toggle;
        s.last_joy_ts       = last_joy_ts;
        s.current_speed_cms = current_speed_cms;
        s.gap_cm            = gap_cm;
        s.status_valid      = status_valid;
        joy_toggle          = false;
        joy_force_manual    = false;
        joy_force_auto      = false;
        joy_emergency_toggle = false;
        return s;
    }
};
