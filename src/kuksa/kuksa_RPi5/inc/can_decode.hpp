#pragma once
#include <cstdint>
#include <linux/can.h> // struct can_frame

namespace can_decode {

// Basic reads
std::uint8_t  u8(const std::uint8_t* p);

std::int16_t  i16_le(const std::uint8_t* p);
std::uint16_t u16_le(const std::uint8_t* p);
std::uint32_t u32_le(const std::uint8_t* p);

// 24-bit little-endian unsigned (3 bytes)
std::uint32_t u24_le(const std::uint8_t* p);

bool dlc_at_least(const can_frame& frame, std::uint8_t n);

} // namespace can_decode
