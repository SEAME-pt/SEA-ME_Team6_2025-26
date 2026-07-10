"""Traffic-light-specific driving rules (independent use case).

This module maps traffic light state to a vehicle motion command.
"""

from __future__ import annotations


VALID_ADVANCE = {"green", "emergency_green"}
VALID_SLOW = {"yellow"}


def decide_motion_from_traffic_light_state(light_state: str) -> str:
    state = str(light_state or "unknown").lower()

    if state in VALID_ADVANCE:
        return "advance"
    if state in VALID_SLOW:
        return "slow_down"

    # Safety-first fallback: red/unknown/timeout -> stop.
    return "stop"
