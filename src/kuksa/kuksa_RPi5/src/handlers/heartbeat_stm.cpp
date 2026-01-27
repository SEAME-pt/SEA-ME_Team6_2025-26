#include "../../inc/handlers.hpp"
#include "../../inc/can_decode.hpp"
#include "../../inc/can_id.h"
#include "../../inc/interface_kuksa_client.hpp"
#include "../../inc/can_to_kuksa_publisher.hpp"
#include "../../inc/signals.hpp"
#include "../../inc/is_stm_connected.hpp"
#include <iostream>
#include <ios>

static std::uint8_t crc_calculate(const std::uint8_t* data, std::size_t len)
{
    std::uint8_t crc = 0x00;
    for (std::size_t i = 0; i < len; ++i) {
        crc ^= data[i];
        for (int b = 0; b < 8; ++b) {
            if (crc & 0x80) crc = (std::uint8_t)((crc << 1) ^ 0x07);
            else           crc = (std::uint8_t)(crc << 1);
        }
    }
    return crc;
}

void handleHeartbeat(const can_frame& frame, IKuksaClient& kuksa)
{
    if (frame.can_dlc < 8)
        return;

    const std::uint8_t state  = can_decode::u8(&frame.data[0]);
    const std::uint32_t up_ms = can_decode::u32_le(&frame.data[1]);
    const std::uint8_t errors = can_decode::u8(&frame.data[5]);
    const std::uint8_t mode   = can_decode::u8(&frame.data[6]);
    const std::uint8_t crc_rx = can_decode::u8(&frame.data[7]);

    const std::uint8_t crc_calc = crc_calculate(frame.data, 7);
    if (crc_calc != crc_rx) {
        std::cerr << "[HB] CRC mismatch calc=0x" << std::hex << (int)crc_calc
                  << " rx=0x" << (int)crc_rx << std::dec << "\n";
        return;
    }

    static std::int32_t hb_counter = 0;
    hb_counter++;

    markHeartbeatSeen();

    kuksa.publishInt32(sig::ECU_SC_HEARTBEAT, hb_counter);

    // ErrorCode: map the 8-bit error flags to a uint32
    kuksa.publishUint32(sig::ECU_SC_ERROR_CODE, static_cast<std::uint32_t>(errors));

    (void)state; (void)up_ms; (void)mode;
}
