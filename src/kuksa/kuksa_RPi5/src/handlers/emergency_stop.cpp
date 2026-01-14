#include "../../inc/handlers.hpp"
#include "../../inc/can_decode.hpp"
#include "../../inc/interface_kuksa_client.hpp"
#include "../../inc/signals.hpp"

void handleEmergencyStop(const can_frame& frame, IKuksaClient& kuksa)
{
    if (frame.can_dlc < 8)
        return;

    const std::uint8_t active = can_decode::u8(&frame.data[0]);
    const std::uint16_t dist_mm = can_decode::u16_le(&frame.data[2]);
    const std::uint8_t reason = can_decode::u8(&frame.data[4]);
    (void)reason;

    //kuksa.publishInt32(sig::ADAS_FRONT_DISTANCE_MM, static_cast<float>(dist_mm));
    kuksa.publishFloat(sig::ADAS_FRONT_DISTANCE_MM, static_cast<float>(dist_mm));
    kuksa.publishBool(sig::ADAS_FRONT_IS_WARNING, active);

    // we can add an emergency stop signal if needed
    //kuksa.publishInt32(sig::ECU_EMERGENCY_STOP_ACTIVE, active ? 1 : 0);
}