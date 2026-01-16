#include "../../inc/handlers.hpp"
#include "../../inc/can_decode.hpp"
#include "../../inc/interface_kuksa_client.hpp"
#include "../../inc/signals.hpp"

static float mg_to_ms2_f(std::int16_t mg)
{
    // mg -> g -> m/s^2
    const float g = static_cast<float>(mg) / 1000.0f;
    return g * 9.80665f;
}

void handleImuAccel(const can_frame& frame, IKuksaClient& kuksa)
{
    if (frame.can_dlc < 8)
        return;

    const std::int16_t ax_mg = can_decode::i16_le(&frame.data[0]);
    const std::int16_t ay_mg = can_decode::i16_le(&frame.data[2]);
    const std::int16_t az_mg = can_decode::i16_le(&frame.data[4]);
    const std::uint8_t status = can_decode::u8(&frame.data[7]);
    (void)status;

    kuksa.publishFloat(sig::ACC_LONGITUDINAL, mg_to_ms2_f(ax_mg));
    kuksa.publishFloat(sig::ACC_LATERAL,      mg_to_ms2_f(ay_mg));
    kuksa.publishFloat(sig::ACC_VERTICAL,     mg_to_ms2_f(az_mg));
}
