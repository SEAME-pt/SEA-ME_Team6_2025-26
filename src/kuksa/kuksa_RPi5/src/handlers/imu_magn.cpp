#include "../../inc/handlers.hpp"
#include "../../inc/can_decode.hpp"
#include "../../inc/interface_kuksa_client.hpp"
#include "../../inc/signals.hpp"

#include <cmath>
#include <cstdint>

static float mg_to_ut(float mg) {
    // 1 Gauss = 100 uT, 1 mG = 0.1 uT
    return mg * 0.1f;
}

void handleImuMag(const can_frame& frame, IKuksaClient& kuksa)
{
    if (frame.can_dlc < 8)
        return;

    const std::int16_t mx_raw = can_decode::i16_le(&frame.data[0]);
    const std::int16_t my_raw = can_decode::i16_le(&frame.data[2]);
    const std::int16_t mz_raw = can_decode::i16_le(&frame.data[4]);
    const std::uint8_t reserved = can_decode::u8(&frame.data[6]);
    const std::uint8_t status   = can_decode::u8(&frame.data[7]);
    (void)reserved;

    // If status bit0 indicates sensor init error, ignore publish
    if (status & 0x01) {
        // optional: publish a fault flag somewhere
        return;
    }

    const float mx_mg = static_cast<float>(mx_raw);
    const float my_mg = static_cast<float>(my_raw);
    const float mz_mg = static_cast<float>(mz_raw);

    // Compute heading in degrees (simple 2D, no tilt compensation)
    // Convention: heading = atan2(Y, X). might need to swap/negate depending on mounting.
    float heading_rad = std::atan2(my_mg, mx_mg);
    float heading_deg = heading_rad * (180.0f / 3.14159265358979323846f);
    if (heading_deg < 0.0f)
        heading_deg += 360.0f;

    kuksa.publishDouble(sig::LOCATION_HEADING, static_cast<double>(heading_deg));
}
