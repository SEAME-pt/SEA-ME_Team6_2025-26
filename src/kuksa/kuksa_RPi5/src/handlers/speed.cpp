#include "../../inc/handlers.hpp"
#include "../../inc/can_decode.hpp"
#include "../../inc/can_id.h"
#include "../../inc/kuksa_client.hpp"
#include "../../inc/can_to_kuksa_publisher.hpp"
#include "../../inc/signals.hpp"


void handleWheelSpeed(const can_frame& frame, KuksaClient& kuksa)
{
    // Expected payload (8 bytes):
    // bytes 0-1: rpm (int16 LE)
    // bytes 2-5: total_pulses (uint32 LE)
    // bytes 6-7: speed_x10 (uint16 LE)  => speed = raw / 10.0

    if (frame.can_dlc < 8)
        return;

    const std::int16_t  rpm       = can_decode::i16_le(&frame.data[0]);   // bytes 0-1
    const std::uint32_t pulses    = can_decode::u32_le(&frame.data[2]);   // bytes 2-5 (optional here)
    const std::uint8_t  direction = can_decode::u8(&frame.data[6]);       // byte 6
    const std::uint8_t  status    = can_decode::u8(&frame.data[7]);       // byte 7
    (void)pulses; (void)direction; (void)status;

    double rpm_signed = static_cast<double>(rpm);
    const double rps = rpm_signed / 60.0;
    const double speed_ms  = rps * WHEEL_PERIMETER;  // wheel_perimeter in meters
    const double speed_kmh = speed_ms * 3.6;

    kuksa.publishDouble(sig::VEHICLE_SPEED, speed_ms);
}
