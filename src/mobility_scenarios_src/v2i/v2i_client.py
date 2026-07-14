#!/usr/bin/env python3
"""Vehicle-side V2I client for Phase 1 MVP.

Simulates vehicle approach, requests barrier opening via a direct local call,
and mirrors status to an abstract KUKSA writer.
"""

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
class VehicleState:
    distance_m: float
    request_sent_at: Optional[float] = None
    retries_done: int = 0
    last_barrier_state: str = "unknown"


class KuksaWriter:
    """Lightweight abstraction; can be swapped by real KUKSA gRPC integration."""

    def __init__(self, enabled: bool, path: str):
        self.enabled = enabled
        self.path = path

    def publish_status(self, value: str) -> None:
        if self.enabled:
            # Placeholder for real gRPC publish integration in Pi5/AGL runtime.
            print(f"[KUKSA] {self.path} = {value}")
        else:
            print(f"[KUKSA-DISABLED] {self.path} = {value}")


class V2IClient:
    def __init__(self, config: dict, barrier_service=None):
        self.config = config
        self.barrier_service = barrier_service

        barrier_id = config["barrier"]["id"]
        v2i_cfg = config["v2i"]
        self.timeout_s = float(v2i_cfg["timeout_ms"]) / 1000.0
        self.max_retries = int(v2i_cfg.get("retry_count", 0))

        veh_cfg = config["vehicle"]
        self.simulation_mode = bool(veh_cfg.get("simulation_mode", True))
        self.trigger_distance_m = float(veh_cfg["trigger_distance_m"])
        self.approach_step_m = float(veh_cfg["approach_step_m"])
        self.loop_interval_s = float(veh_cfg["loop_interval_s"])

        self.state = VehicleState(distance_m=float(veh_cfg["initial_distance_m"]))

        kuksa_cfg = config["kuksa"]
        path = kuksa_cfg["path_template"].format(id=barrier_id)
        self.kuksa = KuksaWriter(enabled=bool(kuksa_cfg.get("enabled", False)), path=path)

    def _on_barrier_status(self, payload: dict) -> None:
        barrier_state = str(payload.get("state", "unknown"))
        self.state.last_barrier_state = barrier_state
        self.state.request_sent_at = None
        self.state.retries_done = 0
        print(f"[V2IClient] Barrier state received: {barrier_state}")
        self.kuksa.publish_status(barrier_state)

    def _send_open_request(self) -> None:
        payload = {
            "action": "open",
            "source": "vehicle",
            "timestamp": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime()),
        }
        self.state.request_sent_at = time.time()
        print(f"[V2IClient] Open request sent: {payload}")
        if self.barrier_service is not None:
            if hasattr(self.barrier_service, "handle_request_via_backend"):
                response = self.barrier_service.handle_request_via_backend(payload)
            else:
                response = self.barrier_service.handle_request(payload)
            self._on_barrier_status(response)

    def _handle_timeout(self) -> None:
        if self.state.request_sent_at is None:
            return
        elapsed = time.time() - self.state.request_sent_at
        if elapsed < self.timeout_s:
            return

        if self.state.retries_done < self.max_retries:
            self.state.retries_done += 1
            print(f"[V2IClient] Timeout waiting barrier response. Retrying {self.state.retries_done}/{self.max_retries}")
            self._send_open_request()
            return

        print("[V2IClient] Timeout reached and retries exhausted. Setting fail-safe state to 'timeout'")
        self.state.request_sent_at = None
        self.state.last_barrier_state = "timeout"
        self.kuksa.publish_status("timeout")

    def _simulate_vehicle_approach(self) -> None:
        if not self.simulation_mode:
            return
        if self.state.distance_m > 0:
            self.state.distance_m = max(0.0, self.state.distance_m - self.approach_step_m)
        print(f"[V2IClient] Simulated distance to barrier: {self.state.distance_m:.1f}m")

    def _should_request_open(self) -> bool:
        return self.state.distance_m <= self.trigger_distance_m and self.state.request_sent_at is None

    def run(self) -> None:
        try:
            while True:
                self._simulate_vehicle_approach()
                if self._should_request_open():
                    self._send_open_request()
                self._handle_timeout()
                time.sleep(self.loop_interval_s)
        except KeyboardInterrupt:
            print("[V2IClient] Stopped by user")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="V2I Vehicle Client")
    parser.add_argument("--config", default="config.json", help="Path to JSON config")
    return parser.parse_args()


def main() -> None:
    args = parse_args()
    config = load_config(args.config)
    app = V2IClient(config)
    app.run()


if __name__ == "__main__":
    main()
