#pragma once
#include <cstdint>
#include <linux/can.h>

class KuksaClient;
void dispatch_can_frame(const can_frame& frame, KuksaClient& kuksa);
