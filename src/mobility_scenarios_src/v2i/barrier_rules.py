"""Barrier-specific driving rules (independent use case).

This module maps barrier state to a vehicle motion command.
"""

from __future__ import annotations


def decide_motion_from_barrier_state(barrier_state: str) -> str:
    state = str(barrier_state or "unknown").lower()

    if state == "open":
        return "advance"

    # Safety-first fallback: any non-open state means the car should stop.
    return "stop"
