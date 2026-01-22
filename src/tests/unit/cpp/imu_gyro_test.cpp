#include <gtest/gtest.h>

#include <cstdint>
#include <linux/can.h>

#include "interface_kuksa_client.hpp"
#include "signals.hpp"
#include "handlers.hpp"
#include "can_encode.hpp"


static float dps01_to_rads_expected(std::int16_t raw)
{
    // Same as imu_gyro.cpp:
    // raw -> dps (raw/10) -> rad/s (dps * pi/180)
    const float dps = static_cast<float>(raw) / 10.0f;
    return dps * 0.017453292519943295f;
}

TEST(ImuGyro, DlcTooShort_PublishesNothing)
{
    FakeKuksaClient k;

    can_frame f{};
    f.can_dlc = 7; // < 8 => early return

    handleImuGyro(f, k);

    EXPECT_TRUE(k.calls.empty());
}

TEST(ImuGyro, ValidFrame_Publishes3AxesInRadPerSecond)
{
    FakeKuksaClient k;

    can_frame f{};
    f.can_dlc = 8;

    // raw units: 0.1 degrees/second (dps01)
    // Choose values with clear meaning:
    //  +100  => 10.0 dps
    //  -50   => -5.0 dps
    //  0     => 0.0 dps
    const std::int16_t gx = 100;
    const std::int16_t gy = -50;
    const std::int16_t gz = 0;

    can_encode::i16_le(&f.data[0], gx);
    can_encode::i16_le(&f.data[2], gy);
    can_encode::i16_le(&f.data[4], gz);
    f.data[7] = 0; // status ignored

    handleImuGyro(f, k);

    // Exactly one publishFloat per signal
    EXPECT_EQ(k.countPathType(sig::ANG_VEL_ROLL,  PublishCall::kFloat), 1u);
    EXPECT_EQ(k.countPathType(sig::ANG_VEL_PITCH, PublishCall::kFloat), 1u);
    EXPECT_EQ(k.countPathType(sig::ANG_VEL_YAW,   PublishCall::kFloat), 1u);

    float roll = 0.0f, pitch = 0.0f, yaw = 0.0f;

    ASSERT_TRUE(k.lastFloat(sig::ANG_VEL_ROLL,  roll));
    ASSERT_TRUE(k.lastFloat(sig::ANG_VEL_PITCH, pitch));
    ASSERT_TRUE(k.lastFloat(sig::ANG_VEL_YAW,   yaw));

    EXPECT_FLOAT_EQ(roll,  dps01_to_rads_expected(gx));
    EXPECT_FLOAT_EQ(pitch, dps01_to_rads_expected(gy));
    EXPECT_FLOAT_EQ(yaw,   dps01_to_rads_expected(gz));
}

TEST(ImuGyro, ZeroRaw_AllAxesPublishZero)
{
    FakeKuksaClient k;

    can_frame f{};
    f.can_dlc = 8;

    can_encode::i16_le(&f.data[0], 0);
    can_encode::i16_le(&f.data[2], 0);
    can_encode::i16_le(&f.data[4], 0);
    f.data[7] = 123; // status ignored

    handleImuGyro(f, k);

    float roll = 1.0f, pitch = 1.0f, yaw = 1.0f;
    ASSERT_TRUE(k.lastFloat(sig::ANG_VEL_ROLL,  roll));
    ASSERT_TRUE(k.lastFloat(sig::ANG_VEL_PITCH, pitch));
    ASSERT_TRUE(k.lastFloat(sig::ANG_VEL_YAW,   yaw));

    EXPECT_FLOAT_EQ(roll,  0.0f);
    EXPECT_FLOAT_EQ(pitch, 0.0f);
    EXPECT_FLOAT_EQ(yaw,   0.0f);
}