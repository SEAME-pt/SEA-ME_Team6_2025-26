#include <gtest/gtest.h>
#include <linux/can.h>
#include <limits>
#include <cmath>

#include "can_frame_builder.hpp"
#include "can_to_kuksa_publisher.hpp"
#include "handlers.hpp"
#include "can_encode.hpp"
#include "can_id.h"
#include "interface_kuksa_client.hpp"
namespace sig {
  static const char* VEHICLE_SPEED = "Vehicle.Speed";
}

#define WHEEL_DIAMETER 0.06675f

// Ignore short DLC frames
TEST(WheelSpeed, IgnoreShortDlc)
{
    can_frame f{};
    f.can_id = CAN_ID_WHEEL_SPEED;
    f.can_dlc = 7;

    FakeKuksaClient k;
    handleWheelSpeed(f, k);

    EXPECT_TRUE(k.calls.empty());
}

// Publishes one double to Vehicle.Speed path
TEST(WheelSpeed, PublishesOneDoubleToVehicleSpeedPath)
{
    can_frame f{};
    f.can_id = CAN_ID_WHEEL_SPEED;
    f.can_dlc = 8;

    can_encode::i16_le(&f.data[0], 60); // 60 rpm

    FakeKuksaClient k;
    handleWheelSpeed(f, k);

    const double rps = 60.0 / 60.0; // rpm to rps (60 rpm = 1 rps)
    const double expected = (rps * WHEEL_PERIMETER) * 1000.0 * 3.6;

    ASSERT_EQ(k.calls.size(), 1u);
    EXPECT_EQ(k.calls[0].type, PublishCall::kDouble);
    EXPECT_EQ(k.calls[0].path, sig::VEHICLE_SPEED);
    EXPECT_NEAR(k.calls[0].d, expected, 1e-9);
}

// Speed  publish a value known
// Speed value is exactly -> wheel perimeter
TEST(WheelSpeed, Rpm60EqualsWheelPerimeterMetersPerHour)
{
    can_frame f{};
    f.can_id = CAN_ID_WHEEL_SPEED;
    f.can_dlc = 8;

    can_encode::i16_le(&f.data[0], 60); // 60 rpm = 1 rps

    FakeKuksaClient k;
    handleWheelSpeed(f, k);

    ASSERT_EQ(k.calls.size(), 1u);

    const double expected = WHEEL_PERIMETER * 1000.0 * 3.6; // because rps=1
    EXPECT_NEAR(k.calls[0].d, expected, 1e-9);
}

// Publishes zero speed when rpm is zero
TEST(WheelSpeed, RpmZeroPublishesZero)
{
    can_frame f{};
    f.can_id = CAN_ID_WHEEL_SPEED;
    f.can_dlc = 8;

    can_encode::i16_le(&f.data[0], 0);

    FakeKuksaClient k;
    handleWheelSpeed(f, k);

    ASSERT_EQ(k.calls.size(), 1u);
    EXPECT_NEAR(k.calls[0].d, 0.0, 1e-12); // this expect_near is for floating-point tolerance
}

// Negative rpm results in negative speed
TEST(WheelSpeed, NegativeRpmIsNegativeSpeed)
{
    can_frame f{};
    f.can_id = CAN_ID_WHEEL_SPEED;
    f.can_dlc = 8;

    can_encode::i16_le(&f.data[0], -60); // -1 rps

    FakeKuksaClient k;
    handleWheelSpeed(f, k);

    ASSERT_EQ(k.calls.size(), 1u);
    EXPECT_NEAR(k.calls[0].d, -WHEEL_PERIMETER * 1000.0 * 3.6, 1e-9);
}

// Changing unused bytes does not affect output speed
TEST(WheelSpeed, ChangingUnusedBytesDoesNotChangeOutput)
{
    can_frame f1{};
    f1.can_id = CAN_ID_WHEEL_SPEED;
    f1.can_dlc = 8;
    can_encode::i16_le(&f1.data[0], 120); // 2 rps

    can_frame f2 = f1;
    f2.can_id = CAN_ID_WHEEL_SPEED;
    // mutate "unused" bytes (2..7) heavily
    for (int i = 2; i < 8; ++i)
        f2.data[i] = static_cast<uint8_t>(0xA0 + i);

    FakeKuksaClient k1, k2;
    handleWheelSpeed(f1, k1);
    handleWheelSpeed(f2, k2);

    ASSERT_EQ(k1.calls.size(), 1u);
    ASSERT_EQ(k2.calls.size(), 1u);
    EXPECT_NEAR(k1.calls[0].d, k2.calls[0].d, 1e-12);
}

TEST(WheelSpeed, HighRpmValue)
{
    can_frame f{};
    f.can_id = CAN_ID_WHEEL_SPEED;
    f.can_dlc = 8;
    std::memset(f.data, 0, sizeof(f.data));

    const int16_t rpm = std::numeric_limits<int16_t>::max();
    can_encode::i16_le(&f.data[0], rpm); // max int16 rpm

    FakeKuksaClient k;
    handleWheelSpeed(f, k);

    ASSERT_EQ(k.calls.size(), 1u);

    EXPECT_TRUE(std::isfinite(k.calls[0].d));

    const double rps = static_cast<double>(rpm) / 60.0;
    const double expected = (rps * WHEEL_PERIMETER) * 1000.0 * 3.6;
    EXPECT_NEAR(k.calls[0].d, expected, 1e-6);
    EXPECT_EQ(k.calls[0].path, sig::VEHICLE_SPEED);
}

TEST(WheelSpeed, LowRpmValue)
{
    can_frame f{};
    f.can_id  = CAN_ID_WHEEL_SPEED;
    f.can_dlc = 8;
    std::memset(f.data, 0, sizeof(f.data));

    const int16_t rpm = std::numeric_limits<int16_t>::min();
    can_encode::i16_le(&f.data[0], rpm);

    FakeKuksaClient k;
    handleWheelSpeed(f, k);

    ASSERT_EQ(k.calls.size(), 1u);
    EXPECT_TRUE(std::isfinite(k.calls[0].d));

    const double rps = static_cast<double>(rpm) / 60.0;
    const double expected = (rps * WHEEL_PERIMETER) * 1000.0 * 3.6;

    EXPECT_NEAR(k.calls[0].d, expected, 1e-6);
    EXPECT_EQ(k.calls[0].path, sig::VEHICLE_SPEED);
}
