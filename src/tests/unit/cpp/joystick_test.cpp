#include <gtest/gtest.h>
#include <cstdint>
#include <cstring>
#include <linux/can.h>

#include "handlers.hpp"
#include "interface_kuksa_client.hpp"
#include "signals.hpp"
#include "can_encode.hpp"

// Helper to create joystick can_frame
static can_frame make_frame(std::int16_t steering, std::int16_t throttle, std::uint8_t dlc = 4)
{
  can_frame f;
  std::memset(&f, 0, sizeof(f));
  f.can_dlc = dlc;
  can_encode::i16_le(&f.data[0], steering);
  can_encode::i16_le(&f.data[2], throttle);
  return f;
}

TEST(JoystickHandler, ReturnsEarlyWhenDlcTooSmall)
{
  FakeKuksaClient kuksa;

  can_frame f = make_frame(10, 20, 3); // dlc < 4
  handleJoystick(f, kuksa);

  EXPECT_TRUE(kuksa.calls.empty());
}

TEST(JoystickHandler, PublishesTwoSignalsWithCorrectTypes)
{
  FakeKuksaClient kuksa;

  can_frame f = make_frame(0, 0, 4);
  handleJoystick(f, kuksa);

  EXPECT_EQ(kuksa.countPathType(sig::CHASSIS_STEER_ANGLE, PublishCall::kInt32), 1u);
  EXPECT_EQ(kuksa.countPathType(sig::CHASSIS_ACCEL_PEDAL, PublishCall::kUint32), 1u);
}

TEST(JoystickHandler, SteeringMinus100MapsToMinus45)
{
  FakeKuksaClient kuksa;

  can_frame f = make_frame(-100, 10);
  handleJoystick(f, kuksa);

  std::int32_t angle = 0;
  ASSERT_TRUE(kuksa.lastInt32(sig::CHASSIS_STEER_ANGLE, angle));
  EXPECT_EQ(angle, -45);

  std::uint32_t accel = 999;
  ASSERT_TRUE(kuksa.lastUint32(sig::CHASSIS_ACCEL_PEDAL, accel));
  EXPECT_EQ(accel, 10u);
}

TEST(JoystickHandler, SteeringPlus100MapsToPlus45)
{
  FakeKuksaClient kuksa;

  can_frame f = make_frame(100, 10);
  handleJoystick(f, kuksa);

  std::int32_t angle = 0;
  ASSERT_TRUE(kuksa.lastInt32(sig::CHASSIS_STEER_ANGLE, angle));
  EXPECT_EQ(angle, 45);
}

TEST(JoystickHandler, SteeringClampsBeyondRange)
{
  FakeKuksaClient kuksa;

  // 200 * 45 / 100 = 90 -> clamp to 45
  can_frame f = make_frame(200, 10);
  handleJoystick(f, kuksa);

  std::int32_t angle = 0;
  ASSERT_TRUE(kuksa.lastInt32(sig::CHASSIS_STEER_ANGLE, angle));
  EXPECT_EQ(angle, 45);
}

TEST(JoystickHandler, SteeringUsesIntegerMath)
{
  FakeKuksaClient kuksa;

  // 33 * 45 / 100 = 14 (integer division)
  can_frame f = make_frame(33, 10);
  handleJoystick(f, kuksa);

  std::int32_t angle = 0;
  ASSERT_TRUE(kuksa.lastInt32(sig::CHASSIS_STEER_ANGLE, angle));
  EXPECT_EQ(angle, 14);
}

TEST(JoystickHandler, ThrottleClampsNegativeToZero)
{
  FakeKuksaClient kuksa;

  can_frame f = make_frame(0, -10);
  handleJoystick(f, kuksa);

  std::uint32_t accel = 999;
  ASSERT_TRUE(kuksa.lastUint32(sig::CHASSIS_ACCEL_PEDAL, accel));
  EXPECT_EQ(accel, 0u);
}

TEST(JoystickHandler, ThrottleClampsAbove100To100)
{
  FakeKuksaClient kuksa;

  can_frame f = make_frame(0, 250);
  handleJoystick(f, kuksa);

  std::uint32_t accel = 0;
  ASSERT_TRUE(kuksa.lastUint32(sig::CHASSIS_ACCEL_PEDAL, accel));
  EXPECT_EQ(accel, 100u);
}