#include "../../inc/handlers.hpp"
#include "../../inc/can_decode.hpp"
#include "../../inc/can_id.h"
#include "../../inc/kuksa_client.hpp"
#include "../../inc/can_to_kuksa_publisher.hpp"
#include "../../inc/signals.hpp"

void handleEnvironment(const can_frame& frame, KuksaClient& kuksa)
{
    // Expected payload (8 bytes):
    // bytes 0-1: temperature_x100 (int16 LE) => temp = raw / 100.0 
    // byte  2 : humidity (uint8)
    // byte  3 : reserved
    // bytes 4-6: pressure (24-bit LE)
    // byte  7 : status

    if (frame.can_dlc < 8)
        return;

    const std::int16_t  raw_temp  = can_decode::i16_le(&frame.data[0]);
    const std::uint8_t  humidity  = can_decode::u8(&frame.data[2]);
    const std::uint8_t  reserved  = can_decode::u8(&frame.data[3]);
    const std::uint32_t pressure  = can_decode::u24_le(&frame.data[4]);
    const std::uint8_t  status    = can_decode::u8(&frame.data[7]);

    (void)humidity;
    (void)reserved;
    (void)pressure;
    (void)status;

    // const double temp = static_cast<double>(raw_temp) / 100.0;
    const double temp = raw_temp / 100.0;

    kuksa.publishDouble(sig::EXT_AIR_TEMPERATURE, temp);
}
