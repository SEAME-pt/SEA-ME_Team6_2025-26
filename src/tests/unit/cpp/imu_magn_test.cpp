#include <gtest/gtest.h>

#include <linux/can.h>
#include <cstdint>
#include <cstring>
#include <map>
#include <string>

#include "interface_kuksa_client.hpp"
#include "signals.hpp"
#include "handlers.hpp"
#include "can_encode.hpp"

static can_frame make_mag_frame(std::int16_t mx, std::int16_t my, std::int16_t mz,
                                std::uint8_t reserved, std::uint8_t status,
                                std::uint8_t dlc)
{
    can_frame f;
    std::memset(&f, 0, sizeof(f));
    f.can_dlc = dlc;

    if (dlc >= 2) can_encode::i16_le(&f.data[0], mx);
    if (dlc >= 4) can_encode::i16_le(&f.data[2], my);
    if (dlc >= 6) can_encode::i16_le(&f.data[4], mz);
    if (dlc >= 7) f.data[6] = reserved;
    if (dlc >= 8) f.data[7] = status;

    return f;
}

// ---------- Tests ----------

TEST(HandleImuMag, Dlclt8_DoesNotPublish)
{
    FakeKuksaClient kuksa;
    // dlc = 7 -> should return early
    can_frame f = make_mag_frame(100, 0, 0, 0, 0, 7);

    handleImuMag(f, kuksa);

    EXPECT_FALSE(kuksa.published);
}

TEST(HandleImuMag, StatusBit0Set_DoesNotPublish)
{
    FakeKuksaClient kuksa;
    // status bit0 indicates sensor init error -> ignore publish
    can_frame f = make_mag_frame(100, 0, 0, 0, 0x01, 8);

    handleImuMag(f, kuksa);

    EXPECT_FALSE(kuksa.published);
}

TEST(HandleImuMag, Heading_ZeroDegrees_WhenY0_XPositive)
{
    FakeKuksaClient kuksa;
    // atan2(0, +100) = 0 deg
    can_frame f = make_mag_frame(100, 0, 0, 0, 0x00, 8);

    handleImuMag(f, kuksa);

    ASSERT_TRUE(kuksa.published);
    EXPECT_EQ(kuksa.lastSignal, sig::LOCATION_HEADING);
    EXPECT_NEAR(kuksa.lastValue, 0.0, 1e-6);
}

TEST(HandleImuMag, Heading_90Degrees_WhenYPositive_X0)
{
    FakeKuksaClient kuksa;
    // atan2(+100, 0) = 90 deg
    can_frame f = make_mag_frame(0, 100, 0, 0, 0x00, 8);

    handleImuMag(f, kuksa);

    ASSERT_TRUE(kuksa.published);
    EXPECT_EQ(kuksa.lastSignal, sig::LOCATION_HEADING);
    EXPECT_NEAR(kuksa.lastValue, 90.0, 1e-6);
}

TEST(HandleImuMag, Heading_180Degrees_WhenY0_XNegative)
{
    FakeKuksaClient kuksa;
    // atan2(0, -100) = 180 deg
    can_frame f = make_mag_frame(-100, 0, 0, 0, 0x00, 8);

    handleImuMag(f, kuksa);

    ASSERT_TRUE(kuksa.published);
    EXPECT_EQ(kuksa.lastSignal, sig::LOCATION_HEADING);
    EXPECT_NEAR(kuksa.lastValue, 180.0, 1e-6);
}

TEST(HandleImuMag, Heading_270Degrees_WhenYNegative_X0_WrapsPositive)
{
    FakeKuksaClient kuksa;
    // atan2(-100, 0) = -90 deg -> +360 => 270 deg
    can_frame f = make_mag_frame(0, -100, 0, 0, 0x00, 8);

    handleImuMag(f, kuksa);

    ASSERT_TRUE(kuksa.published);
    EXPECT_EQ(kuksa.lastSignal, sig::LOCATION_HEADING);
    EXPECT_NEAR(kuksa.lastValue, 270.0, 1e-6);
}

TEST(HandleImuMag, Heading_45Degrees_WhenXPositive_YPositive)
{
    FakeKuksaClient kuksa;
    // atan2(100, 100) = 45 deg
    can_frame f = make_mag_frame(100, 100, 0, 0, 0x00, 8);

    handleImuMag(f, kuksa);

    ASSERT_TRUE(kuksa.published);
    EXPECT_EQ(kuksa.lastSignal, sig::LOCATION_HEADING);
    EXPECT_NEAR(kuksa.lastValue, 45.0, 1e-6);
}