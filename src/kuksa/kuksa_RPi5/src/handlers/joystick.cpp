#include "../../inc/handlers.hpp"
#include "../../inc/can_decode.hpp"
#include "../../inc/interface_kuksa_client.hpp"
#include "../../inc/signals.hpp"

#include <algorithm>
#include <cstdint>


// Limit values to specified range
static inline int clamp_int(int v, int lo, int hi) {
    return std::max(lo, std::min(hi, v));
}

static inline std::uint8_t clamp_u8(int v) {
    return static_cast<std::uint8_t>(clamp_int(v, 0, 100));
}

void handleJoystick(const can_frame& frame, IKuksaClient& kuksa)
{
    // Python sends '<hh' => 4 bytes
    if (frame.can_dlc < 4)
        return;

    const std::int16_t steering_cmd  = can_decode::i16_le(&frame.data[0]);
    const std::int16_t throttle_cmd  = can_decode::i16_le(&frame.data[2]);

    // Map steering -100..100 to -45..45 degrees
    // angle = steering * 45 / 100
    const int angle_deg = clamp_int((steering_cmd * 45) / 100, -45, 45);

    // Map throttle -100..100:
    const int accel_pct = clamp_int(throttle_cmd, 0, 100);

    // Publish to KUKSA
    // SteeringWheel.Angle is int16 but in KUKSA we dont have publishInt16
    kuksa.publishInt32(sig::CHASSIS_STEER_ANGLE, angle_deg);

    // Accelerator pedal is uint8 but we dont have publishUint8
    kuksa.publishUint32(sig::CHASSIS_ACCEL_PEDAL, accel_pct);
}