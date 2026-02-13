#include <gtest/gtest.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <cstdint>

#include "dispatch_frames.hpp"
#include "can_id.h"
#include "interface_kuksa_client.hpp"

// ---------- Spy counters ----------
static int g_speed   = 0;
static int g_env     = 0;
static int g_hb      = 0;
static int g_battery = 0;
static int g_emergcy = 0;
static int g_gyro    = 0;
static int g_tof     = 0;
static int g_accel   = 0;

enum HandlerSlot {
  H_SPEED = 0,
  H_ENV,
  H_HB,
  H_ACCEL,
  H_GYRO,
  H_TOF,
  H_BATT,
  H_EMERG,
  H_COUNT,
  H_MAG
};

static int g_calls[H_COUNT];

static void reset_spies() {
  for (int i = 0; i < H_COUNT; ++i) g_calls[i] = 0;
}


// ---------- Spy handler implementations ----------
// IMPORTANT: These must match the real symbols the dispatcher references.
// Because dispatch_frames.cpp stores function pointers to &handleX,
// linking will resolve them to these implementations in the test binary.

void handleWheelSpeed(const can_frame&, IKuksaClient&)    { ++g_calls[H_SPEED]; }
void handleEnvironment(const can_frame&, IKuksaClient&)   { ++g_calls[H_ENV]; }
void handleHeartbeat(const can_frame&, IKuksaClient&)     { ++g_calls[H_HB]; }
void handleImuAccel(const can_frame&, IKuksaClient&)      { ++g_calls[H_ACCEL]; }
void handleImuGyro(const can_frame&, IKuksaClient&)       { ++g_calls[H_GYRO]; }
void handleToFDistance(const can_frame&, IKuksaClient&)   { ++g_calls[H_TOF]; }
void handleBattery(const can_frame&, IKuksaClient&)       { ++g_calls[H_BATT]; }
void handleEmergencyStop(const can_frame&, IKuksaClient&) { ++g_calls[H_EMERG]; }
void handleImuMag(const can_frame&, IKuksaClient&)        { ++g_calls[H_MAG]; }

// ---------- Tests ----------

struct DispatchCase {
  std::uint32_t can_id;
  HandlerSlot expected;
  const char*  name;
};

class DispatchKnownIdsOnce : public ::testing::TestWithParam<DispatchCase> {};

// Parameterized test for each known CAN ID
// The INSTANTIATE_TEST_SUITE_P below provides the parameters and runs this test for each case.
// Each case checks that the correct handler is called exactly once.
// EXPECT_EQ assertions are used to verify the call counts.
// If EXPECT_EQ fails, it indicates either the expected handler was not called
// If EXPECT_EQ doesnt
TEST_P(DispatchKnownIdsOnce, Dispatch_KnownIdsCallExactlyOneHandlerOnce)
{
  reset_spies();
  FakeKuksaClient k;

  can_frame f{};
  f.can_dlc = 8;
  f.can_id  = GetParam().can_id;

  dispatch_can_frame(f, k);

  // exactly one handler called once
  for (int i = 0; i < H_COUNT; ++i) {
    if (i == (int)GetParam().expected)
      EXPECT_EQ(g_calls[i], 1) << "Expected handler not called: " << GetParam().name;
    else
      EXPECT_EQ(g_calls[i], 0) << "Unexpected handler called: slot=" << i << " case=" << GetParam().name;
  }
}

static DispatchCase kCases[] = {
  { CAN_ID_WHEEL_SPEED,     H_SPEED, "WHEEL_SPEED" },
  { CAN_ID_ENVIRONMENT,     H_ENV,   "ENVIRONMENT" },
  { CAN_ID_HEARTBEAT_STM32, H_HB,    "HEARTBEAT" },
  { CAN_ID_IMU_ACCEL,       H_ACCEL, "IMU_ACCEL" },
  { CAN_ID_IMU_GYRO,        H_GYRO,  "IMU_GYRO" },
  { CAN_ID_IMU_MAG,         H_MAG,   "IMU_MAG" },
  { CAN_ID_TOF_DISTANCE,    H_TOF,   "TOF_DISTANCE" },
  { CAN_ID_BATTERY,         H_BATT,  "BATTERY" },
  { CAN_ID_EMERGENCY_STOP,  H_EMERG, "EMERGENCY_STOP" },
};

INSTANTIATE_TEST_SUITE_P(
  Dispatch,
  DispatchKnownIdsOnce,
  ::testing::ValuesIn(kCases)
);

TEST(Dispatch, Dispatch_DispatchByStandardIdMask_IgnoresFlags)
{
  reset_spies();
  FakeKuksaClient k;

  can_frame f{};
  f.can_dlc = 8;

  // Known ID plus flags; dispatcher should mask with CAN_SFF_MASK and still match.
  f.can_id = (CAN_ID_ENVIRONMENT | CAN_EFF_FLAG | CAN_RTR_FLAG | CAN_ERR_FLAG);

  dispatch_can_frame(f, k);

  EXPECT_EQ(g_calls[H_ENV], 1);
  EXPECT_EQ(g_calls[H_SPEED], 0);
  EXPECT_EQ(g_calls[H_HB], 0);
  EXPECT_EQ(g_calls[H_EMERG], 0);
  EXPECT_EQ(g_calls[H_BATT], 0);
  EXPECT_EQ(g_calls[H_GYRO], 0);
  EXPECT_EQ(g_calls[H_TOF], 0);
  EXPECT_EQ(g_calls[H_ACCEL], 0);
}

TEST(Dispatch, Dispatch_UnknownIdsAreIgnored)
{
  reset_spies();
  FakeKuksaClient k;

  can_frame f{};
  f.can_dlc = 8;

  // ID not known
  f.can_id = 0x7AA;

  dispatch_can_frame(f, k);

  EXPECT_EQ(g_calls[H_SPEED], 0);
  EXPECT_EQ(g_calls[H_ENV], 0);
  EXPECT_EQ(g_calls[H_HB], 0);
  EXPECT_EQ(g_calls[H_EMERG], 0);
  EXPECT_EQ(g_calls[H_BATT], 0);
  EXPECT_EQ(g_calls[H_GYRO], 0);
  EXPECT_EQ(g_calls[H_TOF], 0);
  EXPECT_EQ(g_calls[H_ACCEL], 0);
}
