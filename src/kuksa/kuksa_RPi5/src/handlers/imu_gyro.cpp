#include "../../inc/handlers.hpp"
#include "../../inc/can_decode.hpp"
#include "../../inc/kuksa_client.hpp"
#include "../../inc/signals.hpp"

static float dps01_to_rads_f(std::int16_t raw)
{
    const float dps = static_cast<float>(raw) / 10.0f;
    return dps * 0.017453292519943295f; // pi/180
}

void handleImuGyro(const can_frame& frame, KuksaClient& kuksa)
{
    if (frame.can_dlc < 8)
        return;

    const std::int16_t gx = can_decode::i16_le(&frame.data[0]);
    const std::int16_t gy = can_decode::i16_le(&frame.data[2]);
    const std::int16_t gz = can_decode::i16_le(&frame.data[4]);
    const std::uint8_t status = can_decode::u8(&frame.data[7]);
    (void)status;

    kuksa.publishFloat(sig::ANG_VEL_ROLL,  dps01_to_rads_f(gx));
    kuksa.publishFloat(sig::ANG_VEL_PITCH, dps01_to_rads_f(gy));
    kuksa.publishFloat(sig::ANG_VEL_YAW,   dps01_to_rads_f(gz));
}
