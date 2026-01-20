#include "../inc/is_stm_connected.hpp"
#include "../inc/interface_kuksa_client.hpp"
#include "../inc/signals.hpp"

static std::chrono::steady_clock::time_point g_last_hb =
    std::chrono::steady_clock::time_point::min();

void markHeartbeatSeen()
{
    g_last_hb = std::chrono::steady_clock::now();
}

static bool computeConnected(std::chrono::steady_clock::time_point now)
{
    using namespace std::chrono;
    return (g_last_hb != steady_clock::time_point::min()) &&
           (duration_cast<milliseconds>(now - g_last_hb).count() < 500);
}

void updateIsConnected(IKuksaClient& kuksa)
{
    updateIsConnectedAt(kuksa, std::chrono::steady_clock::now());
}

// -------- test hooks --------

void resetHeartbeatStateForTest()
{
    g_last_hb = std::chrono::steady_clock::time_point::min();
}

void setLastHeartbeatForTest(std::chrono::steady_clock::time_point t)
{
    g_last_hb = t;
}

void updateIsConnectedAt(IKuksaClient& kuksa, std::chrono::steady_clock::time_point now)
{
    const bool connected = computeConnected(now);
    kuksa.publishBool(sig::ECU_SC_IS_CONNECTED, connected);
}
