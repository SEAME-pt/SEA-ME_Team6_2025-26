#include <gtest/gtest.h>
#include <cstdint>
#include "../../kuksa_RPi5/inc/can_decode.hpp"

TEST(CanDecode, REQ_DECODE_001_u8) {
  std::uint8_t b[1] = {0xAB};
  EXPECT_EQ(can_decode::u8(b), 0xAB);
}

TEST(CanDecode, REQ_DECODE_002_u16_le) {
  std::uint8_t b[2] = {0x34, 0x12};
  EXPECT_EQ(can_decode::u16_le(b), 0x1234);
}

TEST(CanDecode, REQ_DECODE_002_i16_le) {
  std::uint8_t b[2] = {0x00, 0x80}; // -32768
  EXPECT_EQ(can_decode::i16_le(b), (std::int16_t)-32768);
}

TEST(CanDecode, REQ_DECODE_003_u32_le) {
  std::uint8_t b[4] = {0x78, 0x56, 0x34, 0x12};
  EXPECT_EQ(can_decode::u32_le(b), 0x12345678u);
}

TEST(CanDecode, REQ_DECODE_004_u24_le) {
  std::uint8_t b[3] = {0x11, 0x22, 0x33};
  EXPECT_EQ(can_decode::u24_le(b), 0x332211u);
}
