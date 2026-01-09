#pragma once
#include <cstdint>
#include <linux/can.h> // struct can_frame

namespace can_decode {

inline std::uint8_t u8(const std::uint8_t* p) { return p[0]; }

inline std::uint16_t u16_le(const std::uint8_t* p) {
    return static_cast<std::uint16_t>(p[0]) |
           (static_cast<std::uint16_t>(p[1]) << 8);
}

inline std::int16_t i16_le(const std::uint8_t* p) {
    return static_cast<std::int16_t>(u16_le(p));
}

inline std::uint32_t u32_le(const std::uint8_t* p) {
    return static_cast<std::uint32_t>(p[0]) |
           (static_cast<std::uint32_t>(p[1]) << 8) |
           (static_cast<std::uint32_t>(p[2]) << 16) |
           (static_cast<std::uint32_t>(p[3]) << 24);
}

inline std::uint32_t u24_le(const std::uint8_t* p) {
    return static_cast<std::uint32_t>(p[0]) |
           (static_cast<std::uint32_t>(p[1]) << 8) |
           (static_cast<std::uint32_t>(p[2]) << 16);
}

inline bool dlc_at_least(const can_frame& frame, std::uint8_t n) {
    return frame.can_dlc >= n;
}

} // namespace can_decode
