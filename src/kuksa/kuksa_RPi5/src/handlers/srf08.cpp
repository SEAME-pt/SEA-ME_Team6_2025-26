#include "../../inc/handlers.hpp"
#include "../../inc/can_decode.hpp"
#include "../../inc/interface_kuksa_client.hpp"
#include "../../inc/signals.hpp"

void handleSrf08(const can_frame& frame, IKuksaClient& kuksa)
{
    if (frame.can_dlc < 8)
        return;

    const std::uint16_t dist_mm    = can_decode::u16_le(&frame.data[0]);
    const std::uint8_t  light      = can_decode::u8(&frame.data[2]);
    const std::uint8_t  status     = can_decode::u8(&frame.data[7]);

    // Only publish if sensor reports a valid reading (bit 0 set)
    if (!(status & 0x01))
        return;

    kuksa.publishFloat(sig::ADAS_FRONT_DISTANCE_MM, static_cast<float>(dist_mm));
    kuksa.publishFloat(sig::EXT_LIGHT_INTENSITY,    static_cast<float>(light));
}
