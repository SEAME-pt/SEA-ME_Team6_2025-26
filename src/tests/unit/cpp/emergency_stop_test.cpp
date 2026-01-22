#include <gtest/gtest.h>

#include <algorithm>
#include <cstdint>
#include <string>

#include <linux/can.h>

#include "interface_kuksa_client.hpp"
#include "signals.hpp"
#include "handlers.hpp"
#include "can_encode.hpp"

TEST(EmergencyStop, DlcTooShort_PublishesNothing)
{
    FakeKuksaClient kuksa;

    can_frame f{};
    f.can_dlc = 7;                          // dlc < 8 => must return
    f.data[0] = 1;                          // would be "active"
    can_encode::u16_le(&f.data[2], 500);    // would be "dist_mm"

    handleEmergencyStop(f, kuksa);

    EXPECT_TRUE(kuksa.calls.empty()); // no publish should happen
}

TEST(EmergencyStop, ValidFrame_PublishesDistanceAndWarning)
{
    FakeKuksaClient kuksa;

    can_frame f{};
    f.can_dlc = 8;

    const std::uint8_t active = 1;
    const std::uint16_t dist_mm = 1234;
    //const std::uint8_t reason = 42; // unused for now

    f.data[0] = active;
    //f.data[4] = reason;
    can_encode::u16_le(&f.data[2], dist_mm);

    handleEmergencyStop(f, kuksa);

    // Expect exactly one float publish for distance and one bool publish for warning
    EXPECT_EQ(kuksa.countPathType(sig::ADAS_FRONT_DISTANCE_MM, PublishCall::kFloat), 1u);
    EXPECT_EQ(kuksa.countPathType(sig::ADAS_FRONT_IS_WARNING,  PublishCall::kBool ), 1u);

    float publishedDist = 0.0f;
    ASSERT_TRUE(kuksa.lastFloat(sig::ADAS_FRONT_DISTANCE_MM, publishedDist));
    EXPECT_FLOAT_EQ(publishedDist, static_cast<float>(dist_mm));

    bool publishedWarn = false;
    ASSERT_TRUE(kuksa.lastBool(sig::ADAS_FRONT_IS_WARNING, publishedWarn));
    EXPECT_EQ(publishedWarn, (active != 0));
}

TEST(EmergencyStop, ActiveZero_PublishesWarningFalse)
{
    FakeKuksaClient kuksa;

    can_frame f{};
    f.can_dlc = 8;

    f.data[0] = 0;                           // active = 0
    can_encode::u16_le(&f.data[2], 10);      // dist_mm = 10
    f.data[4] = 0;

    handleEmergencyStop(f, kuksa);

    bool publishedWarn = true;
    ASSERT_TRUE(kuksa.lastBool(sig::ADAS_FRONT_IS_WARNING, publishedWarn));
    EXPECT_FALSE(publishedWarn);
}
