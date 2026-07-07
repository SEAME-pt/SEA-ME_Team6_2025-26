"""Barrier backend selection for the V2I MVP.

This module keeps the simulator and the future Kitronik hardware adapter behind
one small interface so the rest of the MVP stays unchanged.
"""

from __future__ import annotations

from typing import Protocol


class BarrierBackend(Protocol):
    def handle_request(self, payload: dict) -> dict:
        """Process a barrier request and return the resulting status payload."""


def is_hardware_backend(config: dict) -> bool:
    return str(config.get("barrier", {}).get("backend", "simulation")).lower() == "hardware"
