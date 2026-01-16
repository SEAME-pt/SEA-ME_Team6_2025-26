#pragma once
#include <linux/can.h>
#include <cstring>
#include <cstdint>

inline can_frame make_frame(std::uint32_t id, std::uint8_t dlc) {
  can_frame f;
  std::memset(&f, 0, sizeof(f));
  f.can_id = id;
  f.can_dlc = dlc;
  return f;
}

inline void set_u16_le(can_frame& f, int off, std::uint16_t v) {
  f.data[off + 0] = (std::uint8_t)(v & 0xFF);
  f.data[off + 1] = (std::uint8_t)((v >> 8) & 0xFF);
}

inline void set_i16_le(can_frame& f, int off, std::int16_t v) {
  set_u16_le(f, off, (std::uint16_t)v);
}

inline void set_u24_le(can_frame& f, int off, std::uint32_t v) {
  f.data[off + 0] = (std::uint8_t)(v & 0xFF);
  f.data[off + 1] = (std::uint8_t)((v >> 8) & 0xFF);
  f.data[off + 2] = (std::uint8_t)((v >> 16) & 0xFF);
}
