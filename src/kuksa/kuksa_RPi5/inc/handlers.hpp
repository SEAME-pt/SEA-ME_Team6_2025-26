#pragma once
#include <cstdint>
#include <linux/can.h>

class IKuksaClient;

// function pointer type for handlers - works like hnadler
typedef void (*CanHandlerFn)(const can_frame& frame, IKuksaClient& kuksa);

// declarations
void handleWheelSpeed(const can_frame& frame, IKuksaClient& kuksa);
void handleEnvironment(const can_frame& frame, IKuksaClient& kuksa);
void handleHeartbeat(const can_frame& frame, IKuksaClient& kuksa);

void handleImuAccel(const can_frame& frame, IKuksaClient& kuksa);
void handleImuGyro(const can_frame& frame, IKuksaClient& kuksa);
void handleImuMag(const can_frame& frame, IKuksaClient& kuksa);

void handleToFDistance(const can_frame& frame, IKuksaClient& kuksa);
void handleBattery(const can_frame& frame, IKuksaClient& kuksa);
void handleEmergencyStop(const can_frame& frame, IKuksaClient& kuksa);
void handleJoystick(const can_frame& frame, IKuksaClient& kuksa);