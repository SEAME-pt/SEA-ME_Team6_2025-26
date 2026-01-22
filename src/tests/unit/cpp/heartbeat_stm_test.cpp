#include <gtest/gtest.h>
#include <linux/can.h>
#include <cstdint>

#include "interface_kuksa_client.hpp"
#include "handlers.hpp"
#include "signals.hpp"
#include "can_id.h"
#include "can_to_kuksa_publisher.hpp"
#include "signals.hpp"
#include "is_stm_connected.hpp"

#include <thread>
#include <chrono>
#include <string>

static std::uint8_t crc_calculate(const std::uint8_t* data, std::size_t len)
{
    std::uint8_t crc = 0x00;
    for (std::size_t i = 0; i < len; ++i) {
        crc ^= data[i];
        for (int b = 0; b < 8; ++b) {
            if (crc & 0x80) crc = (std::uint8_t)((crc << 1) ^ 0x07);
            else           crc = (std::uint8_t)(crc << 1);
        }
    }
    return crc;
}

static can_frame makeHbFrameValid(std::uint8_t state,
                                  std::uint32_t up_ms,
                                  std::uint8_t errors,
                                  std::uint8_t mode)
{
    can_frame f{};
    f.can_dlc = 8;

    f.data[0] = state;

    f.data[1] = (std::uint8_t)(up_ms & 0xFF);
    f.data[2] = (std::uint8_t)((up_ms >> 8) & 0xFF);
    f.data[3] = (std::uint8_t)((up_ms >> 16) & 0xFF);
    f.data[4] = (std::uint8_t)((up_ms >> 24) & 0xFF);

    f.data[5] = errors;
    f.data[6] = mode;

    f.data[7] = crc_calculate(f.data, 7);
    return f;
}

static const PublishCall* findLastBool(const FakeKuksaClient& k, const std::string& path)
{
    for (std::size_t i = k.calls.size(); i-- > 0;) {
        if (k.calls[i].type == PublishCall::kBool && k.calls[i].path == path)
            return &k.calls[i];
    }
    return 0;
}

static const PublishCall* findCall(const FakeKuksaClient& k, PublishCall::Type t, const std::string& path)
{
    for (size_t i = 0; i < k.calls.size(); ++i)
        if (k.calls[i].type == t && k.calls[i].path == path)
            return &k.calls[i];
    return 0;
}

TEST(Heartbeat, DlcLessThan8DoesNothing) {
    FakeKuksaClient kuksa;

    can_frame f{};
    f.can_dlc = 7;
    handleHeartbeat(f, kuksa);

    EXPECT_TRUE(kuksa.calls.empty());
}

TEST(Heartbeat, ValidFramePublishesCounterAndErrorCode) {
    FakeKuksaClient kuksa;

    can_frame f = makeHbFrameValid(1, 123, 0xAB, 2);
    handleHeartbeat(f, kuksa);

    // Should publish 2 things on valid frame:
    //  - ECU_SC_HEARTBEAT (int32 counter)
    //  - ECU_SC_ERROR_CODE (uint32 errors)
    const PublishCall* hb = findCall(kuksa, PublishCall::kInt32, sig::ECU_SC_HEARTBEAT);
    const PublishCall* ec = findCall(kuksa, PublishCall::kUint32, sig::ECU_SC_ERROR_CODE);

    ASSERT_NE(hb, (const PublishCall*)0);
    ASSERT_NE(ec, (const PublishCall*)0);
    EXPECT_EQ(ec->u32, (std::uint32_t)0xAB);

    // Don't assert hb->i32 == 1 because hb_counter is static across tests.
    EXPECT_GT(hb->i32, 0);
}

TEST(Heartbeat, CounterIncrementsAcrossValidFrames) {
    FakeKuksaClient k1, k2;

    can_frame f = makeHbFrameValid(1, 1, 0x00, 0);

    handleHeartbeat(f, k1);
    handleHeartbeat(f, k2);

    const PublishCall* hb1 = findCall(k1, PublishCall::kInt32, sig::ECU_SC_HEARTBEAT);
    const PublishCall* hb2 = findCall(k2, PublishCall::kInt32, sig::ECU_SC_HEARTBEAT);
    ASSERT_NE(hb1, (const PublishCall*)0);
    ASSERT_NE(hb2, (const PublishCall*)0);

    EXPECT_LT(hb1->i32, hb2->i32);
}

TEST(Heartbeat, PublishesErrorFlagsAsUint32ToCorrectPath) {
    FakeKuksaClient kuksa;

    can_frame f = makeHbFrameValid(1, 123, 0xAB, 2);
    handleHeartbeat(f, kuksa);

    const PublishCall* ec = findCall(kuksa, PublishCall::kUint32, sig::ECU_SC_ERROR_CODE);
    ASSERT_NE(ec, (const PublishCall*)0);
    EXPECT_EQ(ec->u32, (std::uint32_t)0xAB);
}

TEST(Heartbeat, InvalidCrcPublishesNothing) {
    FakeKuksaClient kuksa;

    can_frame hb = makeHbFrameValid(1, 100, 0x00, 0);
    hb.data[7] ^= 0xFF; // break CRC

    handleHeartbeat(hb, kuksa);

    EXPECT_TRUE(kuksa.calls.empty());
}

TEST(Heartbeat, StateUpMsAndModeDoNotAffectPublishedErrorFlags) {
    FakeKuksaClient k1, k2;

    // same errors, different state/up_ms/mode
    can_frame f1 = makeHbFrameValid(0x01, 100,    0x55, 0x02);
    can_frame f2 = makeHbFrameValid(0xFF, 999999, 0x55, 0xEE);

    handleHeartbeat(f1, k1);
    handleHeartbeat(f2, k2);

    const PublishCall* e1 = findCall(k1, PublishCall::kUint32, sig::ECU_SC_ERROR_CODE);
    const PublishCall* e2 = findCall(k2, PublishCall::kUint32, sig::ECU_SC_ERROR_CODE);
    ASSERT_NE(e1, (const PublishCall*)0);
    ASSERT_NE(e2, (const PublishCall*)0);

    EXPECT_EQ(e1->u32, 0x55u);
    EXPECT_EQ(e2->u32, 0x55u);
}
