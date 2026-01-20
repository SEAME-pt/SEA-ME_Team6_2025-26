#include"../inc/is_stm_connected.hpp"
#include"../inc/interface_kuksa_client.hpp"
#include"../inc/signals.hpp"

static std::chrono::steady_clock::time_point g_last_hb =
    std::chrono::steady_clock::time_point::min();

void markHeartbeatSeen()
{
    g_last_hb = std::chrono::steady_clock::now();
}

void updateIsConnected(IKuksaClient& kuksa)
{
    using namespace std::chrono;
    const auto now = steady_clock::now();

    const bool connected =
        (g_last_hb != steady_clock::time_point::min()) &&
        (duration_cast<milliseconds>(now - g_last_hb).count() < 500);

    kuksa.publishBool(sig::ECU_SC_IS_CONNECTED, connected);
}