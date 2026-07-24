#!/usr/bin/env python3
"""Vehicle-side Emergency Priority client (broker-free MVP)."""

from __future__ import annotations

import argparse
import json
import time
from dataclasses import dataclass
from typing import Optional


def load_config(config_path: str) -> dict:
    with open(config_path, "r", encoding="utf-8") as f:
        return json.load(f)


@dataclass
class EmergencyState:
    priority_active: bool = False
    request_sent_at: Optional[float] = None
    retries_done: int = 0
    last_light_state: str = "unknown"


class KuksaWriter:
    def __init__(self, enabled: bool, priority_path: str, traffic_light_path: str):
        self.enabled = enabled
        self.priority_path = priority_path
        self.traffic_light_path = traffic_light_path

    def publish(self, priority_active: bool, light_state: str) -> None:
        prefix = "[KUKSA]" if self.enabled else "[KUKSA-DISABLED]"
        print(f"{prefix} {self.priority_path} = {priority_active}")
        print(f"{prefix} {self.traffic_light_path} = {light_state}")


class EmergencyClient:
    def __init__(self, config: dict, traffic_light_service=None):
        self.config = config
        self.traffic_light_service = traffic_light_service

        emergency_cfg = config["emergency"]
        self.timeout_s = float(emergency_cfg["timeout_ms"]) / 1000.0
        self.max_retries = int(emergency_cfg.get("retry_count", 0))
        self.source = str(emergency_cfg.get("source", "ambulance"))

        veh_cfg = config["vehicle"]
        self.simulation_mode = bool(veh_cfg.get("simulation_mode", True))
        self.trigger_after_s = float(veh_cfg.get("trigger_after_s", 1.0))
        self.loop_interval_s = float(veh_cfg.get("loop_interval_s", 0.5))

        light_id = config["traffic_light"]["id"]
        kuksa_cfg = config["kuksa"]
        traffic_light_path = kuksa_cfg["traffic_light_path_template"].format(id=light_id)
        self.kuksa = KuksaWriter(
            enabled=bool(kuksa_cfg.get("enabled", False)),
            priority_path=kuksa_cfg["priority_path"],
            traffic_light_path=traffic_light_path,
        )

        self.state = EmergencyState()
        self._started_at = time.time()
        self._triggered = False

    def _on_status(self, response: dict) -> None:
        light_state = str(response.get("state", "unknown"))
        self.state.last_light_state = light_state
        self.state.priority_active = light_state == "emergency_green"
        self.state.request_sent_at = None
        self.state.retries_done = 0
        self.kuksa.publish(self.state.priority_active, light_state)

    def _send_priority_request(self) -> None:
        payload = {
            "action": "emergency_on",
            "source": self.source,
            "timestamp": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime()),
        }
        self.state.request_sent_at = time.time()
        print(f"[EmergencyClient] Priority request sent: {payload}")
        if self.traffic_light_service is not None:
            response = self.traffic_light_service.handle_request(payload)
            self._on_status(response)

    def _handle_timeout(self) -> None:
        if self.state.request_sent_at is None:
            return
        if (time.time() - self.state.request_sent_at) < self.timeout_s:
            return

        if self.state.retries_done < self.max_retries:
            self.state.retries_done += 1
            print(f"[EmergencyClient] Timeout; retry {self.state.retries_done}/{self.max_retries}")
            self._send_priority_request()
            return

        self.state.request_sent_at = None
        self.state.priority_active = False
        self.state.last_light_state = "timeout"
        self.kuksa.publish(False, "timeout")

    def _maybe_auto_trigger(self) -> None:
        if not self.simulation_mode or self._triggered:
            return
        if (time.time() - self._started_at) >= self.trigger_after_s:
            self._triggered = True
            self._send_priority_request()

    def run(self) -> None:
        try:
            while True:
                self._maybe_auto_trigger()
                self._handle_timeout()
                time.sleep(self.loop_interval_s)
        except KeyboardInterrupt:
            print("[EmergencyClient] Stopped by user")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Emergency Priority Vehicle Client")
    parser.add_argument("--config", default="config.json", help="Path to JSON config")
    return parser.parse_args()


def main() -> None:
    args = parse_args()
    config = load_config(args.config)
    app = EmergencyClient(config)
    app.run()


if __name__ == "__main__":
    main()
