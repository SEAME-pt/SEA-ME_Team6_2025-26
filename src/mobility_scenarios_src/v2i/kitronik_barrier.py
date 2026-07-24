"""Placeholder Kitronik barrier adapter.

This is intentionally minimal: the team does not yet have the exact runtime
GPIO/serial integration details for the barrier board at this stage.
The adapter is created now so the project has a clean extension point once the
hardware mapping is confirmed.
"""

from __future__ import annotations

import time


class KitronikBarrierAdapter:
    def __init__(self, barrier_id: str):
        self.barrier_id = barrier_id
        self.state = "closed"

    def handle_request(self, payload: dict) -> dict:
        action = str(payload.get("action", "")).lower()
        timestamp = time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime())

        if action == "open":
            self.state = "open"
        elif action == "close":
            self.state = "closed"

        return {
            "barrier_id": self.barrier_id,
            "state": self.state,
            "timestamp": timestamp,
            "backend": "kitronik-placeholder",
        }
