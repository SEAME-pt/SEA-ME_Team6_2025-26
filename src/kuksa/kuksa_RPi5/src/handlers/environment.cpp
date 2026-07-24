#include "../../inc/handlers.hpp"
#include "../../inc/can_decode.hpp"
#include "../../inc/can_id.h"
#include "../../inc/interface_kuksa_client.hpp"
#include "../../inc/can_to_kuksa_publisher.hpp"
#include "../../inc/signals.hpp"

void handleEnvironment(const can_frame& frame, IKuksaClient& kuksa)
{
    // Expected payload (8 bytes):
    // bytes 0-1: temperature_x100 (int16 LE) => temp = raw / 100.0
    // byte  2 : humidity (uint8)
    // byte  3 : ambient_light_x100 (uint8) => lux = raw * 100.0 (0-255 -> 0-25500 lux)
    // bytes 4-6: pressure (24-bit LE)
    // byte  7 : status

    if (frame.can_dlc < 8)
        return;

    const std::int16_t  raw_temp    = can_decode::i16_le(&frame.data[0]);
    const std::uint8_t  humidity    = can_decode::u8(&frame.data[2]);
    const std::uint8_t  ambient_light_x100 = can_decode::u8(&frame.data[3]);
    const std::uint32_t pressure    = can_decode::u24_le(&frame.data[4]);
    const std::uint8_t  status      = can_decode::u8(&frame.data[7]);

    (void)status;

    const double temp_c = raw_temp / 100.0;
    const double pressure_kpa = static_cast<double>(pressure) / 1000.0;
    const double light_lux = static_cast<double>(ambient_light_x100) * 100.0;

    // publish to KUKSA - VSS patsh
    kuksa.publishDouble(sig::EXT_AIR_TEMPERATURE, temp_c);
    kuksa.publishFloat(sig::EXT_HUMIDITY, static_cast<float>(humidity));
    kuksa.publishFloat(sig::EXT_ATMOS_PRESSURE_KPA, static_cast<float>(pressure_kpa));
    kuksa.publishFloat(sig::EXT_LIGHT_INTENSITY, static_cast<float>(light_lux));
}
