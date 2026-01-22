#include <gtest/gtest.h>
#include <linux/can.h>
#include <cstdint>
#include <string>

#include "can_frame_builder.hpp"

#include "../../kuksa_RPi5/inc/handlers.hpp"
#include "../../kuksa_RPi5/inc/can_encode.hpp"
#include "../../kuksa_RPi5/inc/can_id.h"
#include "../../kuksa_RPi5/inc/interface_kuksa_client.hpp"
#include "../../kuksa_RPi5/inc/signals.hpp"

static const PublishCall* findCall(const FakeKuksaClient& k, PublishCall::Type t, const std::string& path)
{
    for (size_t i = 0; i < k.calls.size(); ++i)
        if (k.calls[i].type == t && k.calls[i].path == path)
            return &k.calls[i];
    return 0;
}

static void put_u24_le(std::uint8_t* p, std::uint32_t v)
{
    v &= 0x00FFFFFFu;
    p[0] = (std::uint8_t)(v & 0xFF);
    p[1] = (std::uint8_t)((v >> 8) & 0xFF);
    p[2] = (std::uint8_t)((v >> 16) & 0xFF);
}

static can_frame makeEnvironmentFrame(std::int16_t temp_x100,
                                      std::uint8_t humidity,
                                      std::uint32_t pressure_pa_24,
                                      std::uint8_t reserved = 0,
                                      std::uint8_t status = 0)
{
    can_frame f = make_frame(CAN_ID_ENVIRONMENT, 8);

    // bytes 0-1: temp_x100 (int16 LE)
    can_encode::i16_le(&f.data[0], temp_x100);

    // byte 2: humidity
    can_encode::u8(&f.data[2], humidity);

    // byte 3: reserved
    can_encode::u8(&f.data[3], reserved);

    // bytes 4-6: pressure (24-bit LE)
    put_u24_le(&f.data[4], pressure_pa_24);

    // byte 7: status
    can_encode::u8(&f.data[7], status);

    return f;
}

TEST(Environment, REQ_ENV_001_IgnoreShortDLC) {
    FakeKuksaClient k;
    can_frame f = make_frame(CAN_ID_ENVIRONMENT, 7);

    handleEnvironment(f, k);

    EXPECT_TRUE(k.calls.empty());
}

TEST(Environment, PublishesExactlyThreeSignalsOnValidFrame) {
    FakeKuksaClient k;

    int16_t temp_x100 = 2534; // 25.34 C
    uint8_t humidity = 55;    // 55 %
    uint32_t pressure_pa = 101325; // 101325 Pa
    can_frame f = makeEnvironmentFrame(temp_x100, humidity, pressure_pa);

    handleEnvironment(f, k);

    EXPECT_EQ(k.calls.size(), 3u);

    // Types + paths exist
    ASSERT_NE(findCall(k, PublishCall::kDouble, sig::EXT_AIR_TEMPERATURE), (const PublishCall*)0);
    ASSERT_NE(findCall(k, PublishCall::kFloat,  sig::EXT_HUMIDITY), (const PublishCall*)0);
    ASSERT_NE(findCall(k, PublishCall::kFloat,  sig::EXT_ATMOS_PRESSURE_KPA), (const PublishCall*)0);
}

TEST(Environment, REQ_ENV_003_ScalesAndPublishesCorrectValues) {
    FakeKuksaClient k;

    int16_t temp_x100 = 2534; // 25.34 C
    uint8_t humidity = 55;    // 55 %
    uint32_t pressure_pa = 101325; // 101325 Pa
    can_frame f = makeEnvironmentFrame(temp_x100, humidity, pressure_pa);

    handleEnvironment(f, k);

    const PublishCall* t  = findCall(k, PublishCall::kDouble, sig::EXT_AIR_TEMPERATURE);
    const PublishCall* h  = findCall(k, PublishCall::kFloat,  sig::EXT_HUMIDITY);
    const PublishCall* pk = findCall(k, PublishCall::kFloat,  sig::EXT_ATMOS_PRESSURE_KPA);

    ASSERT_NE(t,  (const PublishCall*)0);
    ASSERT_NE(h,  (const PublishCall*)0);
    ASSERT_NE(pk, (const PublishCall*)0);

    EXPECT_NEAR(t->d, 25.34, 1e-6);
    EXPECT_NEAR(h->f, 55.0f, 1e-6f);
    EXPECT_NEAR(pk->f, 101.325f, 1e-3f);
}

TEST(Environment, LittleEndianDecodingIsCorrectForTemperature) {
    FakeKuksaClient k;

    // temp_x100 = 0x1234 = 4660 => 46.60 C
    // LE bytes should be 0x34, 0x12
    can_frame f = make_frame(CAN_ID_ENVIRONMENT, 8);
    f.data[0] = 0x34;
    f.data[1] = 0x12;
    f.data[2] = 0;     // humidity
    f.data[3] = 0;     // reserved
    f.data[4] = 0; f.data[5] = 0; f.data[6] = 0; // pressure u24
    f.data[7] = 0;     // status

    handleEnvironment(f, k);

    const PublishCall* t = findCall(k, PublishCall::kDouble, sig::EXT_AIR_TEMPERATURE);
    ASSERT_NE(t, (const PublishCall*)0);
    EXPECT_NEAR(t->d, 46.60, 1e-6);
}

TEST(Environment, LittleEndianDecodingIsCorrectForPressureU24) {
    FakeKuksaClient k;

    // pressure 24-bit: 0x00A1B2C3 = 10597059 Pa => 10597.059 kPa
    // LE bytes: C3 B2 A1
    can_frame f = makeEnvironmentFrame(/*temp*/0, /*hum*/0, /*pressure*/0);
    f.data[4] = 0xC3;
    f.data[5] = 0xB2;
    f.data[6] = 0xA1;

    handleEnvironment(f, k);

    const PublishCall* pk = findCall(k, PublishCall::kFloat, sig::EXT_ATMOS_PRESSURE_KPA);
    ASSERT_NE(pk, (const PublishCall*)0);
    EXPECT_NEAR(pk->f, 10597.059f, 1e-3f);
}

TEST(Environment, ReservedAndStatusBytesDoNotAffectPublishedValues) {
    FakeKuksaClient k1, k2;

    // Same temp/hum/pressure, different reserved/status
    int16_t temp_x100 = 2534; // 25.34 C
    uint8_t humidity = 55;    // 55 %
    uint32_t pressure_pa = 101325; // 101325 Pa
    can_frame f1 = makeEnvironmentFrame(temp_x100, humidity, pressure_pa, /*reserved*/0x00, /*status*/0x00);
    can_frame f2 = makeEnvironmentFrame(temp_x100, humidity, pressure_pa, /*reserved*/0xFF, /*status*/0xAA);

    handleEnvironment(f1, k1);
    handleEnvironment(f2, k2);

    const PublishCall* t1  = findCall(k1, PublishCall::kDouble, sig::EXT_AIR_TEMPERATURE);
    const PublishCall* h1  = findCall(k1, PublishCall::kFloat,  sig::EXT_HUMIDITY);
    const PublishCall* pk1 = findCall(k1, PublishCall::kFloat,  sig::EXT_ATMOS_PRESSURE_KPA);

    const PublishCall* t2  = findCall(k2, PublishCall::kDouble, sig::EXT_AIR_TEMPERATURE);
    const PublishCall* h2  = findCall(k2, PublishCall::kFloat,  sig::EXT_HUMIDITY);
    const PublishCall* pk2 = findCall(k2, PublishCall::kFloat,  sig::EXT_ATMOS_PRESSURE_KPA);

    ASSERT_NE(t1,  (const PublishCall*)0);
    ASSERT_NE(h1,  (const PublishCall*)0);
    ASSERT_NE(pk1, (const PublishCall*)0);
    ASSERT_NE(t2,  (const PublishCall*)0);
    ASSERT_NE(h2,  (const PublishCall*)0);
    ASSERT_NE(pk2, (const PublishCall*)0);

    EXPECT_NEAR(t1->d,  t2->d,  1e-9);
    EXPECT_NEAR(h1->f,  h2->f,  1e-9f);
    EXPECT_NEAR(pk1->f, pk2->f, 1e-6f);
}

TEST(Environment, HandlesTemperatureMinAndMaxWithoutOverflow) {
    FakeKuksaClient kMin, kMax;

    // int16 min/max
    can_frame fMin = makeEnvironmentFrame((std::int16_t)-32768, 0, 0);
    can_frame fMax = makeEnvironmentFrame((std::int16_t) 32767, 0, 0);

    handleEnvironment(fMin, kMin);
    handleEnvironment(fMax, kMax);

    const PublishCall* tMin = findCall(kMin, PublishCall::kDouble, sig::EXT_AIR_TEMPERATURE);
    const PublishCall* tMax = findCall(kMax, PublishCall::kDouble, sig::EXT_AIR_TEMPERATURE);

    ASSERT_NE(tMin, (const PublishCall*)0);
    ASSERT_NE(tMax, (const PublishCall*)0);

    EXPECT_NEAR(tMin->d, -327.68, 1e-6);
    EXPECT_NEAR(tMax->d,  327.67, 1e-6);
}

TEST(Environment, HandlesMax24BitPressureWithoutOverflow) {
    FakeKuksaClient k;

    // max 24-bit value: 0xFFFFFF = 16777215 Pa => 16777.215 kPa
    can_frame f = makeEnvironmentFrame(0, 0, 0x00FFFFFFu);

    handleEnvironment(f, k);

    const PublishCall* pk = findCall(k, PublishCall::kFloat, sig::EXT_ATMOS_PRESSURE_KPA);
    ASSERT_NE(pk, (const PublishCall*)0);
    EXPECT_NEAR(pk->f, 16777.215f, 1e-3f);
}
