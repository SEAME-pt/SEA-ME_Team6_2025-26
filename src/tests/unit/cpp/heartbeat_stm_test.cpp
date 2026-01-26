#include <gtest/gtest.h>
#include <linux/can.h>
#include <cstdint>

#include "../../../kuksa/kuksa_RPi5/inc/interface_kuksa_client.hpp"
#include "../../../kuksa/kuksa_RPi5/inc/handlers.hpp"
#include "../../../kuksa/kuksa_RPi5/inc/signals.hpp"
#include "../../../kuksa/kuksa_RPi5/inc/can_id.h"

static const PublishCall* findInt32(const FakeKuksaClient& k, const std::string& path) {
  for (size_t i = 0; i < k.calls.size(); ++i) {
    if (k.calls[i].type == PublishCall::kInt32 && k.calls[i].path == path)
      return &k.calls[i];
  }
  return 0;
}

// Ignore short DLC frames
TEST(Heartbeat, REQ_HB_001_IgnoreShortDlc)
{
  can_frame f{};
  f.can_dlc = 0;

  FakeKuksaClient k;
  handleHeartbeat(f, k);

  EXPECT_TRUE(k.calls.empty());
}

// Publishes int32 to correct path
TEST(Heartbeat, REQ_HB_003_PublishesInt32ToCorrectPath)
{
  can_frame f{};
  f.can_dlc = 1;
  f.data[0] = 0x2A;

  FakeKuksaClient k;
  handleHeartbeat(f, k);

  ASSERT_EQ(k.calls.size(), 1u);

  const PublishCall* c = findInt32(k, sig::ECU_SC_HEARTBEAT);
  ASSERT_TRUE(c);
  EXPECT_EQ(c->i32, 42);
}

// Decodes byte 0 exactly
TEST(Heartbeat, REQ_HB_002_DecodesByte0Exactly)
{
  can_frame f{};
  f.can_dlc = 1;

  // test a few representative values
  const std::uint8_t values[] = {0x00, 0x01, 0x7F, 0x80, 0xFF};

  for (size_t i = 0; i < sizeof(values)/sizeof(values[0]); ++i) {
    FakeKuksaClient k;
    f.data[0] = values[i];

    handleHeartbeat(f, k);

    ASSERT_EQ(k.calls.size(), 1u);
    const PublishCall* c = findInt32(k, sig::ECU_SC_HEARTBEAT);
    ASSERT_TRUE(c);
    EXPECT_EQ(c->i32, static_cast<std::int32_t>(values[i]));
  }
}

// Extra bytes do not affect output
TEST(Heartbeat, REQ_HB_004_ExtraBytesDoNotAffectOutput)
{
  can_frame f1{};
  f1.can_dlc = 8;
  f1.data[0] = 10;
  for (int i = 1; i < 8; ++i) f1.data[i] = 0x00;

  can_frame f2 = f1;
  for (int i = 1; i < 8; ++i) f2.data[i] = static_cast<std::uint8_t>(0xA0 + i);

  FakeKuksaClient k1, k2;
  handleHeartbeat(f1, k1);
  handleHeartbeat(f2, k2);

  ASSERT_EQ(k1.calls.size(), 1u);
  ASSERT_EQ(k2.calls.size(), 1u);

  const PublishCall* c1 = findInt32(k1, sig::ECU_SC_HEARTBEAT);
  const PublishCall* c2 = findInt32(k2, sig::ECU_SC_HEARTBEAT);

  ASSERT_TRUE(c1);
  ASSERT_TRUE(c2);

  EXPECT_EQ(c1->i32, c2->i32);
  EXPECT_EQ(c1->i32, 10);
}
