#include <gtest/gtest.h>

extern "C" {
  #include <linux/can.h> // struct can_frame
}

#include "handlers.hpp"
#include "signals.hpp"
#include "interface_kuksa_client.hpp"

#ifndef TOF_WARNING_THRESHOLD_MM
#define TOF_WARNING_THRESHOLD_MM 300
#endif

// ---------- Helpers ----------
static can_frame makeToFFrame(uint16_t dist_mm, uint8_t status, uint8_t dlc = 8) {
  can_frame f{};
  f.can_dlc = dlc;

  // distance u16 little-endian at bytes 0..1
  f.data[0] = static_cast<uint8_t>(dist_mm & 0xFF);
  f.data[1] = static_cast<uint8_t>((dist_mm >> 8) & 0xFF);

  // status at byte 7
  f.data[7] = status;
  return f;
}

static const PublishCall* firstCallOfType(const FakeKuksaClient& k, PublishCall::Type t) {
  for (size_t i = 0; i < k.calls.size(); ++i) {
    if (k.calls[i].type == t) return &k.calls[i];
  }
  return 0;
}

static const PublishCall* callAt(const FakeKuksaClient& k, size_t idx) {
  return (idx < k.calls.size()) ? &k.calls[idx] : 0;
}

// ---------- Tests ----------

TEST(handleToFDistance, FrameWithDlcLessThan8DoesNothing) {
  FakeKuksaClient kuksa;
  can_frame f = makeToFFrame(1234, 0, 7);

  handleToFDistance(f, kuksa);

  EXPECT_TRUE(kuksa.calls.empty());
}

TEST(handleToFDistance, ValidFramePublishesToFDistance) {
  FakeKuksaClient kuksa;
  can_frame f = makeToFFrame(1234, 0);

  handleToFDistance(f, kuksa);

  ASSERT_EQ(kuksa.calls.size(), 2u);

  const PublishCall* dist = firstCallOfType(kuksa, PublishCall::kFloat);
  ASSERT_NE(dist, (const PublishCall*)0);
  EXPECT_EQ(dist->path, sig::ADAS_FRONT_DISTANCE_MM);
  EXPECT_FLOAT_EQ(dist->f, 1234.0f);
}

TEST(handleToFDistance, ValidFramePublishesIsWarningBool) {
  FakeKuksaClient kuksa;
  can_frame f = makeToFFrame(299, 0);

  handleToFDistance(f, kuksa);

  ASSERT_EQ(kuksa.calls.size(), 2u);

  const PublishCall* warn = firstCallOfType(kuksa, PublishCall::kBool);
  ASSERT_NE(warn, (const PublishCall*)0);
  EXPECT_EQ(warn->path, sig::ADAS_FRONT_IS_WARNING);
  EXPECT_EQ(warn->b, true);
}

TEST(handleToFDistance, WarningFalseAtOrAboveThreshold) {
  FakeKuksaClient kuksa;

  can_frame f = makeToFFrame(TOF_WARNING_THRESHOLD_MM, 0);
  handleToFDistance(f, kuksa);

  ASSERT_EQ(kuksa.calls.size(), 2u);

  const PublishCall* warn = firstCallOfType(kuksa, PublishCall::kBool);
  ASSERT_NE(warn, (const PublishCall*)0);
  EXPECT_EQ(warn->path, sig::ADAS_FRONT_IS_WARNING);
  EXPECT_EQ(warn->b, false);
}

TEST(handleToFDistance, ZeroDistancePublishesZeroAndWarningTrue) {
  FakeKuksaClient kuksa;
  can_frame f = makeToFFrame(0, 0);

  handleToFDistance(f, kuksa);

  ASSERT_EQ(kuksa.calls.size(), 2u);

  const PublishCall* dist = firstCallOfType(kuksa, PublishCall::kFloat);
  const PublishCall* warn = firstCallOfType(kuksa, PublishCall::kBool);
  ASSERT_NE(dist, (const PublishCall*)0);
  ASSERT_NE(warn, (const PublishCall*)0);

  EXPECT_EQ(dist->path, sig::ADAS_FRONT_DISTANCE_MM);
  EXPECT_FLOAT_EQ(dist->f, 0.0f);

  EXPECT_EQ(warn->path, sig::ADAS_FRONT_IS_WARNING);
  EXPECT_EQ(warn->b, true);
}

TEST(handleToFDistance, LittleEndianDecodingIsCorrect) {
  FakeKuksaClient kuksa;

  can_frame f{};
  f.can_dlc = 8;
  f.data[0] = 0x34;
  f.data[1] = 0x12; // => 0x1234 = 4660
  f.data[7] = 0;

  handleToFDistance(f, kuksa);

  ASSERT_EQ(kuksa.calls.size(), 2u);

  const PublishCall* dist = firstCallOfType(kuksa, PublishCall::kFloat);
  ASSERT_NE(dist, (const PublishCall*)0);
  EXPECT_FLOAT_EQ(dist->f, 4660.0f);
}

TEST(handleToFDistance, MaximumValueDoesNotOverflow) {
  FakeKuksaClient kuksa;
  can_frame f = makeToFFrame(0xFFFF, 0);

  handleToFDistance(f, kuksa);

  ASSERT_EQ(kuksa.calls.size(), 2u);

  const PublishCall* dist = firstCallOfType(kuksa, PublishCall::kFloat);
  const PublishCall* warn = firstCallOfType(kuksa, PublishCall::kBool);
  ASSERT_NE(dist, (const PublishCall*)0);
  ASSERT_NE(warn, (const PublishCall*)0);

  EXPECT_FLOAT_EQ(dist->f, 65535.0f);
  EXPECT_EQ(warn->b, false); // 65535 >= 300
}

TEST(handleToFDistance, MinimumValueDoesNotOverflow) {
  FakeKuksaClient kuksa;
  can_frame f = makeToFFrame(0x0000, 0);

  handleToFDistance(f, kuksa);

  ASSERT_EQ(kuksa.calls.size(), 2u);

  const PublishCall* dist = firstCallOfType(kuksa, PublishCall::kFloat);
  const PublishCall* warn = firstCallOfType(kuksa, PublishCall::kBool);
  ASSERT_NE(dist, (const PublishCall*)0);
  ASSERT_NE(warn, (const PublishCall*)0);

  EXPECT_FLOAT_EQ(dist->f, 0.0f);
  EXPECT_EQ(warn->b, true);
}

TEST(handleToFDistance, CorrectSignalPathsAreUsed) {
  FakeKuksaClient kuksa;
  can_frame f = makeToFFrame(123, 0);

  handleToFDistance(f, kuksa);

  EXPECT_EQ(kuksa.countPath(sig::ADAS_FRONT_DISTANCE_MM), 1u);
  EXPECT_EQ(kuksa.countPath(sig::ADAS_FRONT_IS_WARNING), 1u);
}

TEST(handleToFDistance, PublishIsCalledExactlyTwoTimesPerValidFrame) {
  FakeKuksaClient kuksa;
  can_frame f = makeToFFrame(123, 0);

  handleToFDistance(f, kuksa);

  ASSERT_EQ(kuksa.calls.size(), 2u);

  // exactly one float + one bool
  size_t nFloat = 0, nBool = 0;
  for (size_t i = 0; i < kuksa.calls.size(); ++i) {
    if (kuksa.calls[i].type == PublishCall::kFloat) ++nFloat;
    if (kuksa.calls[i].type == PublishCall::kBool)  ++nBool;
  }
  EXPECT_EQ(nFloat, 1u);
  EXPECT_EQ(nBool, 1u);
}

TEST(handleToFDistance, PublishOrderIsDistanceThenWarning) {
  FakeKuksaClient kuksa;
  can_frame f = makeToFFrame(123, 0);

  handleToFDistance(f, kuksa);

  ASSERT_EQ(kuksa.calls.size(), 2u);

  const PublishCall* c0 = callAt(kuksa, 0);
  const PublishCall* c1 = callAt(kuksa, 1);
  ASSERT_NE(c0, (const PublishCall*)0);
  ASSERT_NE(c1, (const PublishCall*)0);

  EXPECT_EQ(c0->type, PublishCall::kFloat);
  EXPECT_EQ(c0->path, sig::ADAS_FRONT_DISTANCE_MM);

  EXPECT_EQ(c1->type, PublishCall::kBool);
  EXPECT_EQ(c1->path, sig::ADAS_FRONT_IS_WARNING);
}

TEST(handleToFDistance, StatusByteDoesNotAffectBehavior) {
  FakeKuksaClient k1;
  FakeKuksaClient k2;

  can_frame f1 = makeToFFrame(250, 0x00);
  can_frame f2 = makeToFFrame(250, 0xFF);

  handleToFDistance(f1, k1);
  handleToFDistance(f2, k2);

  ASSERT_EQ(k1.calls.size(), 2u);
  ASSERT_EQ(k2.calls.size(), 2u);

  // Compare values (same distance and warning)
  EXPECT_EQ(k1.calls[0].type, k2.calls[0].type);
  EXPECT_EQ(k1.calls[0].path, k2.calls[0].path);
  EXPECT_FLOAT_EQ(k1.calls[0].f, k2.calls[0].f);

  EXPECT_EQ(k1.calls[1].type, k2.calls[1].type);
  EXPECT_EQ(k1.calls[1].path, k2.calls[1].path);
  EXPECT_EQ(k1.calls[1].b, k2.calls[1].b);
}

// "Negative dist_mm" note:
// With current code (u16 decode), negative isn't representable.
// This test proves we never publish a negative float from 0xFFFF.
TEST(handleToFDistance, DistanceCannotBeNegativeWithUnsignedDecode) {
  FakeKuksaClient kuksa;
  can_frame f = makeToFFrame(0xFFFF, 0);

  handleToFDistance(f, kuksa);

  ASSERT_EQ(kuksa.calls.size(), 2u);
  const PublishCall* dist = firstCallOfType(kuksa, PublishCall::kFloat);
  ASSERT_NE(dist, (const PublishCall*)0);

  EXPECT_GE(dist->f, 0.0f);
}
