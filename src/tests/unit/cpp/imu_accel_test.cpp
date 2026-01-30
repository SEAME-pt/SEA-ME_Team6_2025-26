#include <gtest/gtest.h>

#include <cstdint>
#include <linux/can.h>

#include "interface_kuksa_client.hpp"
#include "signals.hpp"
#include "handlers.hpp"
#include "can_encode.hpp"

static float mg_to_ms2_expected(std::int16_t mg)
{
    // Same logic as imu_accel.cpp:
    // mg -> g -> m/s^2
    const float g = static_cast<float>(mg) / 1000.0f;
    return g * 9.80665f;
}

TEST(ImuAccel, DlcTooShort_PublishesNothing)
{
    FakeKuksaClient k;

    can_frame f{};
    f.can_dlc = 7; // dlc < 8 => early return

    handleImuAccel(f, k);

    EXPECT_TRUE(k.calls.empty());
}

TEST(ImuAccel, ValidFrame_Publishes3Axes)
{
    FakeKuksaClient k;

    can_frame f{};
    f.can_dlc = 8;

    const std::int16_t ax_mg = 1000;  // 1g
    const std::int16_t ay_mg = 0;     // 0g
    const std::int16_t az_mg = -500;  // -0.5g

    can_encode::i16_le(&f.data[0], ax_mg);
    can_encode::i16_le(&f.data[2], ay_mg);
    can_encode::i16_le(&f.data[4], az_mg);
    f.data[7] = 0; // status (ignored for now)

    handleImuAccel(f, k);

    // Exactly one publishFloat per signal
    EXPECT_EQ(k.countPathType(sig::ACC_LONGITUDINAL, PublishCall::kFloat), 1u);
    EXPECT_EQ(k.countPathType(sig::ACC_LATERAL,      PublishCall::kFloat), 1u);
    EXPECT_EQ(k.countPathType(sig::ACC_VERTICAL,     PublishCall::kFloat), 1u);

    // Validate values (use EXPECT_FLOAT_EQ for exact float comparison in this deterministic math)
    float ax = 0.0f;
    float ay = 0.0f;
    float az = 0.0f;

    ASSERT_TRUE(k.lastFloat(sig::ACC_LONGITUDINAL, ax));
    ASSERT_TRUE(k.lastFloat(sig::ACC_LATERAL,      ay));
    ASSERT_TRUE(k.lastFloat(sig::ACC_VERTICAL,     az));

    EXPECT_FLOAT_EQ(ax, mg_to_ms2_expected(ax_mg));
    EXPECT_FLOAT_EQ(ay, mg_to_ms2_expected(ay_mg));
    EXPECT_FLOAT_EQ(az, mg_to_ms2_expected(az_mg));
}

TEST(ImuAccel, ZeroG_AllAxesPublishZero)
{
    FakeKuksaClient k;

    can_frame f{};
    f.can_dlc = 8;

    can_encode::i16_le(&f.data[0], 0);
    can_encode::i16_le(&f.data[2], 0);
    can_encode::i16_le(&f.data[4], 0);
    f.data[7] = 123; // status (ignored for now)

    handleImuAccel(f, k);

    float ax = 1.0f, ay = 1.0f, az = 1.0f;
    ASSERT_TRUE(k.lastFloat(sig::ACC_LONGITUDINAL, ax));
    ASSERT_TRUE(k.lastFloat(sig::ACC_LATERAL,      ay));
    ASSERT_TRUE(k.lastFloat(sig::ACC_VERTICAL,     az));

    EXPECT_FLOAT_EQ(ax, 0.0f);
    EXPECT_FLOAT_EQ(ay, 0.0f);
    EXPECT_FLOAT_EQ(az, 0.0f);
}