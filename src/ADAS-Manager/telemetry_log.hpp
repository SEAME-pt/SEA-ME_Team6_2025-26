#pragma once

#include "adas_state.hpp"
#include "drive_mode.hpp"
#include "socket_receiver.hpp"
#include "oa_controller.hpp"
#include <cstdint>
#include <cstdio>

// ── Tick log ─────────────────────────────────────────────────────────────────
static void log_tick(
    AdasState adas_state,
    DriveMode drive_mode,
    OAController& oa,
    bool lane_valid,
    const LaneFrame& lane,
    bool obj_valid,
    const ObjectFrame& obj,
    bool v2i_valid,
    const V2IFrame& v2i,
    int steering,
    int throttle,
    int throttle_limit,
    int default_throttle,
    bool acc_active,
    uint16_t target_speed_cms,
    uint16_t current_speed_cms,
    bool status_valid)
{
    const char* lane_str = (lane.lane_status < 4)
                           ? LANE_STATUS_STR[lane.lane_status] : "?";

    printf("[ADAS][%-14s][%s][OA:%s] ", state_str(adas_state),
           drive_mode == DriveMode::MANUAL ? "MANUAL" : "AUTO  ",
           oa_state_str(oa.state()));

    if (lane_valid)
        printf("lane=%-5s  dev=%+.2f  steer=%+4d  throttle=%3d",
               lane_str, lane.lateral_deviation, steering, throttle);
    else
        printf("lane=---              steer=%+4d  throttle=%3d",
               steering, throttle);

    if (acc_active)
        printf("  ACC→%.1fkmh", target_speed_cms * 3.6f / 100.0f);
    if (status_valid)
        printf("  spd=%.1fkmh", current_speed_cms * 3.6f / 100.0f);

    if (obj_valid && obj.count > 0) {
        printf("  | obj=%u", obj.count);
        for (uint8_t i = 0; i < obj.count && i < MAX_OBJECTS; ++i)
            printf("  [cls=%u conf=%.2f dist=%.2fm theta=%+.1fdeg]",
                   obj.objects[i].class_id,
                   obj.objects[i].confidence,
                   obj.objects[i].distance,
                   obj.objects[i].theta_cam);
        if (throttle_limit < default_throttle)
            printf("  *** THROTTLE OVERRIDE=%d ***", throttle_limit);
        printf("\n");
    } else {
        printf("  | obj=---\n");
    }

    if (v2i_valid) {
        printf("[ADAS][V2I] tl=%u barrier=%u priority=%u\n",
               static_cast<unsigned>(v2i.traffic_light_state),
               static_cast<unsigned>(v2i.barrier_state),
               static_cast<unsigned>(v2i.priority_active));
    }
}
