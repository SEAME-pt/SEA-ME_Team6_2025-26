#!/usr/bin/env python3
"""Simple local barrier simulator for Phase 1 V2I MVP.

This version is broker-free: the vehicle-side code calls it directly and
the barrier responds with a status payload.
"""

from __future__ import annotations

import argparse
import json
import threading
import time
from dataclasses import dataclass

from barrier_backend import is_hardware_backend
from kitronik_barrier import KitronikBarrierAdapter


@dataclass
class BarrierState:
    barrier_id: str
    state: str = "closed"
    last_change_ts: float = 0.0


def load_config(config_path: str) -> dict:
    with open(config_path, "r", encoding="utf-8") as f:
        return json.load(f)


class BarrierSimulator:
    def __init__(self, config: dict):
        self.config = config
        barrier_id = config["barrier"]["id"]
        self.state = BarrierState(barrier_id=barrier_id, state="closed", last_change_ts=time.time())

        self.open_duration_s = float(config["barrier"]["open_duration_s"])
        self.backend = KitronikBarrierAdapter(barrier_id) if is_hardware_backend(config) else None

        self._lock = threading.Lock()

    def _status_payload(self) -> dict:
        return {
            "barrier_id": self.state.barrier_id,
            "state": self.state.state,
            "timestamp": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime(self.state.last_change_ts)),
        }

    def _publish_status(self) -> dict:
        payload = self._status_payload()
        print(f"[BarrierSimulator] Status: {payload}")
        return payload

    def _set_state(self, new_state: str) -> None:
        with self._lock:
            self.state.state = new_state
            self.state.last_change_ts = time.time()
        self._publish_status()

    def _auto_close(self) -> None:
        time.sleep(self.open_duration_s)
        self._set_state("closed")

    def _handle_open_request(self) -> None:
        self._set_state("open")
        t = threading.Thread(target=self._auto_close, daemon=True)
        t.start()

    def handle_request(self, payload: dict) -> dict:
        action = str(payload.get("action", "")).lower()
        if action == "open":
            print(f"[BarrierSimulator] Open requested: {payload}")
            self._handle_open_request()
        elif action == "close":
            print(f"[BarrierSimulator] Close requested: {payload}")
            self._set_state("closed")
        else:
            print(f"[BarrierSimulator] Unknown action '{action}'")
        return self._status_payload()

    def handle_request_via_backend(self, payload: dict) -> dict:
        """Keep one entry-point while allowing a future hardware adapter."""
        if self.backend is None:
            return self.handle_request(payload)
        response = self.backend.handle_request(payload)
        self.state.state = response.get("state", self.state.state)
        self.state.last_change_ts = time.time()
        print(f"[BarrierSimulator] Backend response: {response}")
        return response

    def run(self) -> None:
        print("[BarrierSimulator] Local simulator ready. No broker required.")
        self._publish_status()
        while True:
            time.sleep(1.0)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="V2I Barrier Simulator")
    parser.add_argument("--config", default="config.json", help="Path to JSON config")
    return parser.parse_args()


def main() -> None:
    args = parse_args()
    config = load_config(args.config)
    simulator = BarrierSimulator(config)
    simulator.run()


if __name__ == "__main__":
    main()
