#include <gtest/gtest.h>
#include <linux/can.h>
#include <cstdint>

#include "../../kuksa_RPi5/inc/interface_kuksa_client.hpp"
#include "../../kuksa_RPi5/inc/handlers.hpp"
#include "../../kuksa_RPi5/inc/signals.hpp"
#include "../../kuksa_RPi5/inc/can_encode.hpp"
#include "../../kuksa_RPi5/inc/can_id.h"

static const PublishCall* findCall(const FakeKuksaClient& k, PublishCall::Type t, const std::string& path) {
  for (size_t i = 0; i < k.calls.size(); ++i)
    if (k.calls[i].type == t && k.calls[i].path == path) return &k.calls[i];
  return 0;
}

// Ignore short DLC frames
TEST(Battery, REQ_BATT_001_IgnoreShortDLC)
{
  can_frame f{};
  f.can_id = CAN_ID_BATTERY;
  f.can_dlc = 7;

  FakeKuksaClient k;
  handleBattery(f, k);

  EXPECT_TRUE(k.calls.empty());
}

// Publishes exactly 5 signals - voltage, current, soc, is_level_low, is_critical
TEST(Battery, REQ_BATT_006_PublishesExactly5Signals)
{
  can_frame f{};
  f.can_id = CAN_ID_BATTERY;
  f.can_dlc = 8;

  can_encode::u16_le(&f.data[0], 12000);   // 12.000 V
  can_encode::i16_le(&f.data[2], 1500);    // 1.500 A
  can_encode::u8(&f.data[4], 50);          // 50 %
  can_encode::u8(&f.data[5], 25);          // temp (unused)
  can_encode::u8(&f.data[6], 2);           // cycles (unused)
  can_encode::u8(&f.data[7], 0);           // status

  FakeKuksaClient k;
  handleBattery(f, k);

  ASSERT_EQ(k.calls.size(), 5u);

  EXPECT_TRUE(findCall(k, PublishCall::kFloat, sig::TBATT_VOLT_V));
  EXPECT_TRUE(findCall(k, PublishCall::kFloat, sig::TBATT_CURR_A));
  EXPECT_TRUE(findCall(k, PublishCall::kFloat, sig::TBATT_SOC_CURRENT));
  EXPECT_TRUE(findCall(k, PublishCall::kBool,  sig::TBATT_IS_LEVEL_LOW));
  EXPECT_TRUE(findCall(k, PublishCall::kBool,  sig::TBATT_IS_CRITICAL));
}

// Voltage and current are scaled correctly
TEST(Battery, REQ_BATT_002_VoltageAndCurrentScaling)
{
  can_frame f{};
  f.can_id = CAN_ID_BATTERY;
  f.can_dlc = 8;

  can_encode::u16_le(&f.data[0], 12345);    // 12.345 V
  can_encode::i16_le(&f.data[2], -250);     // -0.250 A
  can_encode::u8(&f.data[4], 80);
  can_encode::u8(&f.data[5], 0);
  can_encode::u8(&f.data[6], 0);
  can_encode::u8(&f.data[7], 0);

  FakeKuksaClient k;
  handleBattery(f, k);

  const PublishCall* v = findCall(k, PublishCall::kFloat, sig::TBATT_VOLT_V);
  const PublishCall* c = findCall(k, PublishCall::kFloat, sig::TBATT_CURR_A);

  ASSERT_TRUE(v); ASSERT_TRUE(c);
  EXPECT_NEAR(v->f, 12.345f, 1e-5f);
  EXPECT_NEAR(c->f, -0.250f, 1e-5f);
}

// SOC is published as float correctly
TEST(Battery, REQ_BATT_003_PublishesSocAsFloat)
{
  can_frame f{};
  f.can_id = CAN_ID_BATTERY;
  f.can_dlc = 8;

  can_encode::u16_le(&f.data[0], 1000);
  can_encode::i16_le(&f.data[2], 0);
  can_encode::u8(&f.data[4], 42);
  can_encode::u8(&f.data[5], 0);
  can_encode::u8(&f.data[6], 0);
  can_encode::u8(&f.data[7], 0);

  FakeKuksaClient k;
  handleBattery(f, k);

  const PublishCall* s = findCall(k, PublishCall::kFloat, sig::TBATT_SOC_CURRENT);
  ASSERT_TRUE(s);
  EXPECT_NEAR(s->f, 42.0f, 1e-5f);
}

// Low is true if status bit 2 is set
TEST(Battery, REQ_BATT_004_LowIsTrueIfStatusBit2Set)
{
  can_frame f{};
  f.can_id = CAN_ID_BATTERY;
  f.can_dlc = 8;

  can_encode::u16_le(&f.data[0], 12000);
  can_encode::i16_le(&f.data[2], 0);
  can_encode::u8(&f.data[4], 80);       // soc high
  can_encode::u8(&f.data[5], 0);
  can_encode::u8(&f.data[6], 0);
  can_encode::u8(&f.data[7], (1 << 2)); // low battery flag

  FakeKuksaClient k;
  handleBattery(f, k);

  const PublishCall* low = findCall(k, PublishCall::kBool, sig::TBATT_IS_LEVEL_LOW);
  ASSERT_TRUE(low);
  EXPECT_TRUE(low->b);
}

// Low is true if soc below 20%
TEST(Battery, REQ_BATT_004_LowIsTrueIfSocBelow20)
{
  can_frame f{};
  f.can_id = CAN_ID_BATTERY;
  f.can_dlc = 8;

  can_encode::u16_le(&f.data[0], 12000);
  can_encode::i16_le(&f.data[2], 0);
  can_encode::u8(&f.data[4], 19); // < 20
  can_encode::u8(&f.data[5], 0);
  can_encode::u8(&f.data[6], 0);
  can_encode::u8(&f.data[7], 0);  // status bit2 clear

  FakeKuksaClient k;
  handleBattery(f, k);

  const PublishCall* low = findCall(k, PublishCall::kBool, sig::TBATT_IS_LEVEL_LOW);
  ASSERT_TRUE(low);
  EXPECT_TRUE(low->b);
}

// Critical is true if status bit 0 is set
TEST(Battery, REQ_BATT_005_CriticalIsTrueIfStatusBit0Set)
{
  can_frame f{};
  f.can_id = CAN_ID_BATTERY;
  f.can_dlc = 8;

  can_encode::u16_le(&f.data[0], 12000);
  can_encode::i16_le(&f.data[2], 0);
  can_encode::u8(&f.data[4], 50);       // >=10
  can_encode::u8(&f.data[5], 0);
  can_encode::u8(&f.data[6], 0);
  can_encode::u8(&f.data[7], (1 << 0)); // undervoltage

  FakeKuksaClient k;
  handleBattery(f, k);

  const PublishCall* crit = findCall(k, PublishCall::kBool, sig::TBATT_IS_CRITICAL);
  ASSERT_TRUE(crit);
  EXPECT_TRUE(crit->b);
}

// Critical is true if soc below 10%
TEST(Battery, REQ_BATT_005_CriticalIsTrueIfSocBelow10)
{
  can_frame f{};
  f.can_id = CAN_ID_BATTERY;
  f.can_dlc = 8;

  can_encode::u16_le(&f.data[0], 12000);
  can_encode::i16_le(&f.data[2], 0);
  can_encode::u8(&f.data[4], 9); // < 10
  can_encode::u8(&f.data[5], 0);
  can_encode::u8(&f.data[6], 0);
  can_encode::u8(&f.data[7], 0); // status bit0 clear

  FakeKuksaClient k;
  handleBattery(f, k);

  const PublishCall* crit = findCall(k, PublishCall::kBool, sig::TBATT_IS_CRITICAL);
  ASSERT_TRUE(crit);
  EXPECT_TRUE(crit->b);
}
