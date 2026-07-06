#include "../../inc/handlers.hpp"
#include "../../inc/can_decode.hpp"
#include "../../inc/interface_kuksa_client.hpp"
#include "../../inc/signals.hpp"

#ifndef TOF_WARNING_THRESHOLD_MM
#define TOF_WARNING_THRESHOLD_MM 300
#endif

void handleToFDistance(const can_frame& frame, IKuksaClient& kuksa)
{
    if (frame.can_dlc < 8)
        return;

    const std::uint16_t dist_mm = can_decode::u16_le(&frame.data[0]);
    const std::uint8_t status   = can_decode::u8(&frame.data[7]);
    (void)status;

    kuksa.publishFloat(sig::ADAS_FRONT_DISTANCE_MM, static_cast<float>(dist_mm));

    const bool is_warning = (dist_mm < TOF_WARNING_THRESHOLD_MM) ? true : false;
    kuksa.publishBool(sig::ADAS_FRONT_IS_WARNING, is_warning);
}
