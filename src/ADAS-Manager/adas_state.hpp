#pragma once

enum class AdasState { INIT, ACTIVE, DEGRADED, EMERGENCY_STOP };

static const char* state_str(AdasState s) {
    switch (s) {
        case AdasState::INIT:           return "INIT";
        case AdasState::ACTIVE:         return "ACTIVE";
        case AdasState::DEGRADED:       return "DEGRADED";
        case AdasState::EMERGENCY_STOP: return "EMERGENCY_STOP";
    }
    return "?";
}
