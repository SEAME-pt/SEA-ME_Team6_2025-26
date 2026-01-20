#include <chrono>
#include "interface_kuksa_client.hpp"

void markHeartbeatSeen();
void updateIsConnected(IKuksaClient& kuksa);

// Test hooks (safe, tiny)
void resetHeartbeatStateForTest();
void setLastHeartbeatForTest(std::chrono::steady_clock::time_point t);
void updateIsConnectedAt(IKuksaClient& kuksa, std::chrono::steady_clock::time_point now);
