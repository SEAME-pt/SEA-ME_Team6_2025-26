#include <gtest/gtest.h>
#include <chrono>
#include <string>

#include "../../kuksa_RPi5/inc/is_stm_connected.hpp"
#include "../../kuksa_RPi5/inc/interface_kuksa_client.hpp"
#include "../../kuksa_RPi5/inc/signals.hpp"

// Helper: find last published bool for a path
static const PublishCall* findLastBool(const FakeKuksaClient& k, const std::string& path)
{
    for (std::size_t i = k.calls.size(); i-- > 0;) {
        if (k.calls[i].type == PublishCall::kBool && k.calls[i].path == path)
            return &k.calls[i];
    }
    return 0;
}

static std::chrono::steady_clock::time_point tp_ms(long long ms)
{
    using clock = std::chrono::steady_clock;
    return clock::time_point(std::chrono::milliseconds(ms));
}

TEST(IsConnected, ResetStateMakesDisconnected) {
    FakeKuksaClient k;
    resetHeartbeatStateForTest();

    updateIsConnectedAt(k, tp_ms(1000));

    const PublishCall* c = findLastBool(k, sig::ECU_SC_IS_CONNECTED);
    ASSERT_NE(c, (const PublishCall*)0);
    EXPECT_EQ(c->b, false);
}

TEST(IsConnected, ConnectedWhenHeartbeatSeenWithin500ms) {
    FakeKuksaClient k;
    resetHeartbeatStateForTest();

    setLastHeartbeatForTest(tp_ms(1000));

    // 100ms later => connected
    updateIsConnectedAt(k, tp_ms(1100));

    const PublishCall* c = findLastBool(k, sig::ECU_SC_IS_CONNECTED);
    ASSERT_NE(c, (const PublishCall*)0);
    EXPECT_EQ(c->b, true);
}

TEST(IsConnected, DisconnectedAtOrAfter500msBoundary) {
    FakeKuksaClient k;
    resetHeartbeatStateForTest();

    setLastHeartbeatForTest(tp_ms(1000));

    // exactly 500ms later => NOT connected because code uses < 500
    updateIsConnectedAt(k, tp_ms(1500));

    const PublishCall* c = findLastBool(k, sig::ECU_SC_IS_CONNECTED);
    ASSERT_NE(c, (const PublishCall*)0);
    EXPECT_EQ(c->b, false);
}

TEST(IsConnected, DisconnectedAfterTimeout) {
    FakeKuksaClient k;
    resetHeartbeatStateForTest();

    setLastHeartbeatForTest(tp_ms(1000));

    // 600ms later => disconnected
    updateIsConnectedAt(k, tp_ms(1600));

    const PublishCall* c = findLastBool(k, sig::ECU_SC_IS_CONNECTED);
    ASSERT_NE(c, (const PublishCall*)0);
    EXPECT_EQ(c->b, false);
}

TEST(IsConnected, PublishesToCorrectPathExactlyOncePerCall) {
    FakeKuksaClient k;
    resetHeartbeatStateForTest();

    // No heartbeat => should publish false once
    updateIsConnectedAt(k, tp_ms(2000));

    // Exactly one publish
    ASSERT_EQ(k.calls.size(), 1u);
    EXPECT_EQ(k.calls[0].type, PublishCall::kBool);
    EXPECT_EQ(k.calls[0].path, std::string(sig::ECU_SC_IS_CONNECTED));
    EXPECT_EQ(k.calls[0].b, false);
}

TEST(IsConnected, MultipleCallsPublishMultipleSamples) {
    FakeKuksaClient k;
    resetHeartbeatStateForTest();

    setLastHeartbeatForTest(tp_ms(1000));

    updateIsConnectedAt(k, tp_ms(1100)); // true
    updateIsConnectedAt(k, tp_ms(2000)); // false

    ASSERT_EQ(k.calls.size(), 2u);
    EXPECT_EQ(k.calls[0].type, PublishCall::kBool);
    EXPECT_EQ(k.calls[1].type, PublishCall::kBool);
    EXPECT_EQ(k.calls[0].path, std::string(sig::ECU_SC_IS_CONNECTED));
    EXPECT_EQ(k.calls[1].path, std::string(sig::ECU_SC_IS_CONNECTED));
    EXPECT_EQ(k.calls[0].b, true);
    EXPECT_EQ(k.calls[1].b, false);
}

TEST(IsConnected, Wrapper_UpdateIsConnected_IsTrueRightAfterMarkSeen) {
    FakeKuksaClient k;
    resetHeartbeatStateForTest();

    markHeartbeatSeen();   // sets g_last_hb = now
    updateIsConnected(k);  // uses now again (almost same time)

    const PublishCall* c = findLastBool(k, sig::ECU_SC_IS_CONNECTED);
    ASSERT_NE(c, (const PublishCall*)0);
    EXPECT_EQ(c->b, true);
}