#include <gtest/gtest.h>
#include <linux/can.h>

#include "can_frame_builder.hpp"

#include "../../kuksa_RPi5/inc/handlers.hpp"
#include "../../kuksa_RPi5/inc/can_encode.hpp"
#include "../../kuksa_RPi5/inc/can_id.h"
#include "../../kuksa_RPi5/inc/interface_kuksa_client.hpp"

namespace sig {
  static const char* EXT_AIR_TEMPERATURE = "Vehicle.Exterior.AirTemperature";
  static const char* EXT_HUMIDITY = "Vehicle.Exterior.Humidity";
  static const char* EXT_ATMOS_PRESSURE_KPA = "Vehicle.Exterior.AtmosphericPressure";
}

// Ignore short DLC frames
TEST(Environment, REQ_ENV_001_IgnoreShortDLC) {
  FakeKuksaClient k;
  can_frame f = make_frame(CAN_ID_ENVIRONMENT, 7);
  handleEnvironment(f, k);
  EXPECT_TRUE(k.calls.empty());
}

// Publishes three values to respective paths
TEST(Environment, REQ_ENV_003_ScalesAndPublishes) {
  FakeKuksaClient k;
  can_frame f = make_frame(CAN_ID_ENVIRONMENT, 8);

  // raw_temp = 2534 => 25.34 C
  can_encode::i16_le(&f.data[0], (std::int16_t)2534);
  f.data[2] = 55; // humidity
  // pressure raw = 101325 => 101.325 kPa if /1000
  can_encode::u32_le(&f.data[4], 101325);

  handleEnvironment(f, k);

  EXPECT_EQ(k.countPath(sig::EXT_AIR_TEMPERATURE), 1u);
  EXPECT_EQ(k.countPath(sig::EXT_HUMIDITY), 1u);
  EXPECT_EQ(k.countPath(sig::EXT_ATMOS_PRESSURE_KPA), 1u);

  // verify values (find calls)
  double temp = 0; float hum = 0; float pkpa = 0;
  for (size_t i = 0; i < k.calls.size(); ++i) {
    if (k.calls[i].path == sig::EXT_AIR_TEMPERATURE) temp = k.calls[i].d;
    if (k.calls[i].path == sig::EXT_HUMIDITY) hum = k.calls[i].f;
    if (k.calls[i].path == sig::EXT_ATMOS_PRESSURE_KPA) pkpa = k.calls[i].f;
  }

  EXPECT_NEAR(temp, 25.34, 1e-6);
  EXPECT_NEAR(hum, 55.0f, 1e-6f);
  EXPECT_NEAR(pkpa, 101.325f, 1e-3f);
}
