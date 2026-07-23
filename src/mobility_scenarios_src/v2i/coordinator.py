#!/usr/bin/env python3
"""Coordinator for normal V2I traffic-light scenario.

This keeps the normal vehicle behavior separate from emergency-priority logic.
"""

from __future__ import annotations

from dataclasses import dataclass
from enum import IntEnum


class VehicleMode(IntEnum):
    NORMAL = 0
    EMERGENCY = 1


@dataclass
class RoadScenarioInput:
    vehicle_mode: VehicleMode
    approaching: bool
    same_lane: bool
    traffic_light_state: str
    barrier_state: str


@dataclass
class RoadScenarioResult:
    vehicle_motion: str
    priority_active: bool
    nearest_traffic_light_action: str
    other_traffic_lights_action: str
    barrier_action: str
    street_lights_action: str
    ambulance_siren_action: str


def _motion_from_traffic_light(light_state: str) -> str:
    state = str(light_state or "unknown").lower()
    if state in {"green", "emergency_green"}:
        return "advance"
    if state == "yellow":
        return "slow_down"
    return "stop"


def _motion_from_barrier(barrier_state: str) -> str:
    state = str(barrier_state or "unknown").lower()
    if state == "open":
        return "advance"
    return "stop"


class Coordinator:
    def __init__(self, traffic_light_service=None, barrier_service=None):
        self.traffic_light_service = traffic_light_service
        self.barrier_service = barrier_service

    def resolve_road_scenario(self, data: RoadScenarioInput) -> RoadScenarioResult:
        """Resolve behavior for normal vehicle V2I and emergency override."""
        if (
            data.vehicle_mode == VehicleMode.EMERGENCY
            and data.approaching
            and data.same_lane
        ):
            return RoadScenarioResult(
                vehicle_motion="advance",
                priority_active=True,
                nearest_traffic_light_action="blink_all",
                other_traffic_lights_action="red",
                barrier_action="open",
                street_lights_action="blink",
                ambulance_siren_action="on",
            )

        tl_motion = _motion_from_traffic_light(data.traffic_light_state)
        barrier_motion = _motion_from_barrier(data.barrier_state)
        final_motion = "advance"
        if barrier_motion == "stop":
            final_motion = "stop"
        elif tl_motion == "stop":
            final_motion = "stop"
        elif tl_motion == "slow_down":
            final_motion = "slow_down"

        return RoadScenarioResult(
            vehicle_motion=final_motion,
            priority_active=False,
            nearest_traffic_light_action=str(data.traffic_light_state or "red").lower(),
            other_traffic_lights_action="normal",
            barrier_action=str(data.barrier_state or "closed").lower(),
            street_lights_action="normal",
            ambulance_siren_action="off",
        )