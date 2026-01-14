#include "../../inc/handlers.hpp"
#include "../../inc/can_decode.hpp"
#include "../../inc/kuksa_client.hpp"
#include "../../inc/signals.hpp"

void handleBattery(const can_frame& frame, KuksaClient& kuksa)
{
    if (frame.can_dlc < 8)
        return;

    const std::uint16_t voltage_mv = can_decode::u16_le(&frame.data[0]);
    const std::int16_t  current_ma = can_decode::i16_le(&frame.data[2]);
    const std::uint8_t  soc        = can_decode::u8(&frame.data[4]);
    const std::int8_t   temp_c     = (std::int8_t)can_decode::u8(&frame.data[5]);
    const std::uint8_t  cycles     = can_decode::u8(&frame.data[6]);
    const std::uint8_t  status     = can_decode::u8(&frame.data[7]);
    (void)temp_c;
    (void)cycles;
    (void)status;

    const float volt_v = static_cast<float>(voltage_mv) / 1000.0f;
    const float curr_a = static_cast<float>(current_ma) / 1000.0f;
    const float soc_f  = static_cast<float>(soc);

    kuksa.publishFloat(sig::TBATT_VOLT_V, volt_v);
    kuksa.publishFloat(sig::TBATT_CURR_A, curr_a);
    kuksa.publishFloat(sig::TBATT_SOC_CURRENT, soc_f);

    // Status bits from your STM32 sim:
    // bit0 undervoltage, bit1 overvoltage, bit2 low battery, bit3 high temp
    const bool low = ((status & (1 << 2)) != 0) || (soc < 20);
    const bool critical  = ((status & (1 << 0)) != 0) || (soc < 10);

    kuksa.publishBool(sig::TBATT_IS_LEVEL_LOW, low);
    kuksa.publishBool(sig::TBATT_IS_CRITICAL,  critical);

}
