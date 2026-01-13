#pragma once
#include <cstdint>
#include <linux/can.h>

class KuksaClient;

// function pointer type for handlers - works like hnadler
typedef void (*CanHandlerFn)(const can_frame& frame, KuksaClient& kuksa);

// declarations
void handleWheelSpeed(const can_frame& frame, KuksaClient& kuksa);
void handleEnvironment(const can_frame& frame, KuksaClient& kuksa);
void handleHeartbeat(const can_frame& frame, KuksaClient& kuksa);
