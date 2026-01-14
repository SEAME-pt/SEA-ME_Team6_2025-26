#pragma once
#include <cstdint>
#include <linux/can.h>

class IKuksaClient;
void dispatch_can_frame(const can_frame& frame, IKuksaClient& kuksa);
