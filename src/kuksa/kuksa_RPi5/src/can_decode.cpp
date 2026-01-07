#include "can_decode.hpp"

namespace can_decode {

std::uint8_t u8(const std::uint8_t* p) {
    return p[0];
}

std::int16_t i16_le(const std::uint8_t* p) {
    std::uint16_t v = (std::uint16_t)p[0] | ((std::uint16_t)p[1] << 8);
    return (std::int16_t)v;
}

std::uint16_t u16_le(const std::uint8_t* p) {
    return (std::uint16_t)p[0] | ((std::uint16_t)p[1] << 8);
}

std::uint32_t u32_le(const std::uint8_t* p) {
    return (std::uint32_t)p[0]
        | ((std::uint32_t)p[1] << 8)
        | ((std::uint32_t)p[2] << 16)
        | ((std::uint32_t)p[3] << 24);
}

std::uint32_t u24_le(const std::uint8_t* p) {
    // Little-endian 24-bit: p[0] is LSB, p[2] is MSB
    return (std::uint32_t)p[0]
        | ((std::uint32_t)p[1] << 8)
        | ((std::uint32_t)p[2] << 16);
}

bool dlc_at_least(const can_frame& frame, std::uint8_t n) {
    return frame.can_dlc >= n;
}

} // namespace can_decode
