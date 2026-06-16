#!/usr/bin/env python3
"""Keyboard-driven scenario toggle for Module 3 roadside logic.

Controls (single key press):
  1 -> vehicle mode NORMAL (car)
  2 -> vehicle mode EMERGENCY (ambulance)
  a -> toggle approaching flag
  l -> toggle same-lane flag
  r/y/g -> set traffic light state (red/yellow/green)
  o/m/c -> set barrier state (open/mid/closed)
  q -> quit

This script is designed for Lenovo keyboard use during demos.
"""

from __future__ import annotations

import sys
import termios
import tty
from dataclasses import dataclass

from coordinator import Coordinator, RoadScenarioInput, VehicleMode


@dataclass
class SessionState:
    mode: VehicleMode = VehicleMode.NORMAL
    approaching: bool = False
    same_lane: bool = True
    traffic_light_state: str = "red"
    barrier_state: str = "closed"


def render_help() -> None:
    print("\n=== Keyboard Scenario Toggle ===")
    print("1: NORMAL (car)    2: EMERGENCY (ambulance)")
    print("a: approaching     l: same-lane")
    print("r/y/g: traffic light red/yellow/green")
    print("o/m/c: barrier open/mid/closed")
    print("q: quit")


def render_state(state: SessionState) -> None:
    print(
        "\n[INPUT] "
        f"mode={state.mode.name} approaching={state.approaching} same_lane={state.same_lane} "
        f"traffic_light={state.traffic_light_state} barrier={state.barrier_state}"
    )


def render_result(controller: Coordinator, state: SessionState) -> None:
    result = controller.resolve_road_scenario(
        RoadScenarioInput(
            vehicle_mode=state.mode,
            approaching=state.approaching,
            same_lane=state.same_lane,
            traffic_light_state=state.traffic_light_state,
            barrier_state=state.barrier_state,
        )
    )
    print(
        "[OUTPUT] "
        f"priority={result.priority_active} motion={result.vehicle_motion} "
        f"nearest_tl={result.nearest_traffic_light_action} other_tls={result.other_traffic_lights_action} "
        f"barrier={result.barrier_action} street_lights={result.street_lights_action} "
        f"ambulance_siren={result.ambulance_siren_action}"
    )


def main() -> int:
    controller = Coordinator(traffic_light_service=None, barrier_service=None)
    state = SessionState()

    render_help()
    render_state(state)
    render_result(controller, state)

    fd = sys.stdin.fileno()
    old_settings = termios.tcgetattr(fd)
    try:
        tty.setcbreak(fd)
        while True:
            ch = sys.stdin.read(1)
            if not ch:
                continue

            if ch == "q":
                print("\n[EXIT] keyboard scenario toggle stopped")
                return 0
            if ch == "1":
                state.mode = VehicleMode.NORMAL
            elif ch == "2":
                state.mode = VehicleMode.EMERGENCY
            elif ch == "a":
                state.approaching = not state.approaching
            elif ch == "l":
                state.same_lane = not state.same_lane
            elif ch == "r":
                state.traffic_light_state = "red"
            elif ch == "y":
                state.traffic_light_state = "yellow"
            elif ch == "g":
                state.traffic_light_state = "green"
            elif ch == "o":
                state.barrier_state = "open"
            elif ch == "m":
                state.barrier_state = "mid"
            elif ch == "c":
                state.barrier_state = "closed"
            else:
                continue

            render_state(state)
            render_result(controller, state)
    finally:
        termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)


if __name__ == "__main__":
    raise SystemExit(main())
