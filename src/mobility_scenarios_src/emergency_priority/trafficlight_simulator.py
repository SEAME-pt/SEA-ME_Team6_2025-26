#!/usr/bin/env python3
"""Local traffic light simulator for Emergency Priority Phase 2."""

from __future__ import annotations

import argparse
import json
import threading
import time
from dataclasses import dataclass


@dataclass
class TrafficLightState:
    light_id: str
    state: str = "red"
    last_change_ts: float = 0.0


def load_config(config_path: str) -> dict:
    with open(config_path, "r", encoding="utf-8") as f:
        return json.load(f)


class TrafficLightSimulator:
    def __init__(self, config: dict):
        self.config = config
        light_id = config["traffic_light"]["id"]
        self.state = TrafficLightState(light_id=light_id, state="red", last_change_ts=time.time())
        self.green_duration_s = float(config["traffic_light"].get("emergency_green_duration_s", 5.0))
        self._lock = threading.Lock()

    def _status_payload(self) -> dict:
        return {
            "traffic_light_id": self.state.light_id,
            "state": self.state.state,
            "timestamp": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime(self.state.last_change_ts)),
        }

    def _set_state(self, new_state: str) -> None:
        with self._lock:
            self.state.state = new_state
            self.state.last_change_ts = time.time()
        print(f"[TrafficLightSimulator] Status: {self._status_payload()}")

    def _auto_return_red(self) -> None:
        time.sleep(self.green_duration_s)
        self._set_state("red")

    def handle_request(self, payload: dict) -> dict:
        action = str(payload.get("action", "")).lower()
        if action == "emergency_on":
            self._set_state("emergency_green")
            threading.Thread(target=self._auto_return_red, daemon=True).start()
        elif action == "yellow":
            self._set_state("yellow")
        elif action == "green":
            self._set_state("green")
        elif action == "emergency_off":
            self._set_state("red")
        return self._status_payload()


class TrafficLightHardwarePlaceholder:
    def __init__(self, light_id: str):
        self.light_id = light_id
        self.state = "red"

    def handle_request(self, payload: dict) -> dict:
        action = str(payload.get("action", "")).lower()
        if action == "emergency_on":
            self.state = "emergency_green"
        elif action == "yellow":
            self.state = "yellow"
        elif action == "green":
            self.state = "green"
        elif action == "emergency_off":
            self.state = "red"
        return {
            "traffic_light_id": self.light_id,
            "state": self.state,
            "timestamp": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime()),
            "backend": "hardware-placeholder",
        }


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Emergency Priority Traffic Light Simulator")
    parser.add_argument("--config", default="config.json", help="Path to JSON config")
    return parser.parse_args()


def main() -> None:
    args = parse_args()
    config = load_config(args.config)
    sim = TrafficLightSimulator(config)
    print("[TrafficLightSimulator] Ready (local mode).")
    print(sim._status_payload())


if __name__ == "__main__":
    main()
