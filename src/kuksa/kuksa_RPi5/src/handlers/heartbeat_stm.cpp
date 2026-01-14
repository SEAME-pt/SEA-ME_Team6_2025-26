#include "../../inc/handlers.hpp"
#include "../../inc/can_decode.hpp"
#include "../../inc/can_id.h"
#include "../../inc/interface_kuksa_client.hpp"
#include "../../inc/can_to_kuksa_publisher.hpp"
#include "../../inc/signals.hpp"

void handleHeartbeat(const can_frame& frame, IKuksaClient& kuksa)
{
    // Expected payload:
    // byte 0: heartbeat counter / value

    if (frame.can_dlc < 1)
        return;

    const std::uint8_t hb = can_decode::u8(&frame.data[0]);

    // Publish as int32 (common to avoid int8 type mismatch in KUKSA Value oneof)
    kuksa.publishInt32(sig::ECU_SC_HEARTBEAT, static_cast<std::int32_t>(hb));
}
