#include "../../inc/handlers.hpp"
#include "../../inc/can_decode.hpp"
#include "../../inc/can_id.h"
#include "../../inc/interface_kuksa_client.hpp"
#include "../../inc/can_to_kuksa_publisher.hpp"
#include "../../inc/signals.hpp"

void handleAEB(const can_frame& frame, IKuksaClient& kuksa) {
    // Expected payload (2 bytes):
    // bytes 0: warn (uint8)
    // bytes 1: brake (uint8)

    // if (frame.can_dlc < 2)
    //     return;

    const std::uint8_t  warn    = can_decode::u8(&frame.data[0]);
    const std::uint8_t  brake    = can_decode::u8(&frame.data[1]);

    kuksa.publishBool(sig::ADAS_FRONT_IS_WARNING, warn);
    kuksa.publishBool(sig::ADAS_FRONT_IS_BRAKING, brake);
}