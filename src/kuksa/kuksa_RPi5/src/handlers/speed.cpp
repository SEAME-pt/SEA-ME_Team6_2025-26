#include "../../inc/handlers.hpp"
#include "../../inc/can_decode.hpp"
#include "../../inc/can_id.h"
#include "../../inc/interface_kuksa_client.hpp"
#include "../../inc/can_to_kuksa_publisher.hpp"
#include "../../inc/signals.hpp"


void handleWheelSpeed(const can_frame& frame, IKuksaClient& kuksa)
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
    (void)direction; (void)status;

    //const std::int16_t rpm = can_decode::i16_le(&frame.data[0]);

    double rpm_signed = static_cast<double>(rpm);
    const double rps = rpm_signed / 60.0;
    const double speed_mh  = (rps * WHEEL_PERIMETER) * 1000.0 * 3.6;
    const double speed_kmh = speed_mh * 3.6;

    kuksa.publishDouble(sig::VEHICLE_SPEED, speed_mh);
}
