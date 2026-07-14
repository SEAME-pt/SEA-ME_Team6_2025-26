#!/usr/bin/env python3
"""Unified local demo for Module 3 (V2I + Emergency Priority).

Runs a deterministic sequence:
1) Normal mode -> traffic light red, barrier closed.
2) Emergency mode -> traffic light emergency_green, barrier open.
3) Back to normal -> traffic light red, barrier closed.

This script is designed to be demo evidence without requiring physical hardware.
"""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path


THIS_DIR = Path(__file__).resolve().parent
V2I_DIR = (THIS_DIR / "../v2i").resolve()
if str(V2I_DIR) not in sys.path:
    sys.path.insert(0, str(V2I_DIR))

from barrier_simulator import BarrierSimulator  # noqa: E402
from coordinator import Coordinator  # noqa: E402
from trafficlight_simulator import TrafficLightSimulator  # noqa: E402


def load_config(config_path: str) -> dict:
    with open(config_path, "r", encoding="utf-8") as f:
        return json.load(f)


def build_barrier_config() -> dict:
    return {
        "barrier": {"id": "demo_barrier_1", "backend": "simulation", "open_duration_s": 0.5},
        "v2i": {"timeout_ms": 1000, "retry_count": 0},
        "vehicle": {
            "simulation_mode": True,
            "initial_distance_m": 10.0,
            "trigger_distance_m": 5.0,
            "approach_step_m": 1.0,
            "loop_interval_s": 0.1,
        },
        "kuksa": {"enabled": False, "path_template": "Vehicle.Infra.Barrier.{id}.Status"},
    }


def log_step(title: str, result) -> None:
    print(f"\n=== {title} ===")
    print(f"priority_active: {result.priority_active}")
    print(f"traffic_light_state: {result.traffic_light_state}")
    print(f"barrier_state: {result.barrier_state}")


def run_demo(config: dict) -> int:
    light = TrafficLightSimulator(config)
    barrier = BarrierSimulator(build_barrier_config())
    coordinator = Coordinator(light, barrier)

    # Step 1: normal mode
    normal = coordinator.handle_event(emergency_active=False, normal_request_active=False)
    log_step("STEP 1 - Normal Mode", normal)

    # Step 2: emergency mode
    emergency = coordinator.handle_event(emergency_active=True, normal_request_active=False)
    log_step("STEP 2 - Emergency Mode", emergency)

    # Step 3: return to normal
    back_to_normal = coordinator.handle_event(emergency_active=False, normal_request_active=False)
    log_step("STEP 3 - Back To Normal", back_to_normal)

    ok = (
        normal.priority_active is False
        and normal.traffic_light_state == "red"
        and normal.barrier_state == "closed"
        and emergency.priority_active is True
        and emergency.traffic_light_state == "emergency_green"
        and emergency.barrier_state == "open"
        and back_to_normal.priority_active is False
        and back_to_normal.traffic_light_state == "red"
        and back_to_normal.barrier_state == "closed"
    )

    print("\n=== DEMO RESULT ===")
    print("PASS" if ok else "FAIL")
    return 0 if ok else 1


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Unified local demo for V2I + Emergency Priority")
    parser.add_argument("--config", default="config.json", help="Path to Emergency Priority JSON config")
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    config = load_config(args.config)
    return run_demo(config)


if __name__ == "__main__":
    raise SystemExit(main())
