#pragma once
#include <cstdint>

namespace can_encode {

inline void u8(std::uint8_t* p, std::uint8_t v) {
    p[0] = v;
}

inline void u16_le(std::uint8_t* p, std::uint16_t v) {
    p[0] = static_cast<std::uint8_t>(v & 0xFF);
    p[1] = static_cast<std::uint8_t>((v >> 8) & 0xFF);
}

inline void i16_le(std::uint8_t* p, std::int16_t v) {
    u16_le(p, static_cast<std::uint16_t>(v));
}

inline void u32_le(std::uint8_t* p, std::uint32_t v) {
    p[0] = static_cast<std::uint8_t>(v & 0xFF);
    p[1] = static_cast<std::uint8_t>((v >> 8) & 0xFF);
    p[2] = static_cast<std::uint8_t>((v >> 16) & 0xFF);
    p[3] = static_cast<std::uint8_t>((v >> 24) & 0xFF);
}

} // namespace can_encode
