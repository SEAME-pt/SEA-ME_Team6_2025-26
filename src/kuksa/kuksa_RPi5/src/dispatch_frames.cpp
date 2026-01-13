#include "../inc/dispatch_frames.hpp"
#include "../inc/handlers.hpp"
#include "can_id.h"
#include <linux/can.h>

struct Entry { std::uint32_t id; CanHandlerFn fn; };

static const Entry kHandlers[] = {
    { CAN_ID_WHEEL_SPEED,      &handleWheelSpeed },
    { CAN_ID_ENVIRONMENT,      &handleEnvironment },
    { CAN_ID_HEARTBEAT_STM32,  &handleHeartbeat },
};

static const std::size_t kHandlersCount = sizeof(kHandlers) / sizeof(kHandlers[0]);

void dispatch_can_frame(const can_frame& frame, KuksaClient& kuksa)
{
    const std::uint32_t id = frame.can_id & CAN_SFF_MASK;

    for (std::size_t i = 0; i < kHandlersCount; ++i) {
        if (kHandlers[i].id == id) {
            kHandlers[i].fn(frame, kuksa);
            return;
        }
    }
    // unknown ID ignored
}
