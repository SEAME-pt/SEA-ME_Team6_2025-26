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

void handleImuAccel(const can_frame& frame, KuksaClient& kuksa);
void handleImuGyro(const can_frame& frame, KuksaClient& kuksa);
//void handleImuMag(const can_frame& frame, KuksaClient& kuksa);

void handleToFDistance(const can_frame& frame, KuksaClient& kuksa);
void handleBattery(const can_frame& frame, KuksaClient& kuksa);
void handleEmergencyStop(const can_frame& frame, KuksaClient& kuksa);