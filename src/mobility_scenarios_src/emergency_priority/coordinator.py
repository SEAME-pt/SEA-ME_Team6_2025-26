#!/usr/bin/env python3
"""Unified coordinator scaffold for Phase 3.

For now it focuses on emergency override policy and local orchestration hooks.
"""

from __future__ import annotations

from dataclasses import dataclass
from enum import IntEnum
from typing import Optional


@dataclass
class CoordinatorResult:
    priority_active: bool
    traffic_light_state: str
    barrier_state: Optional[str] = None


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
    # Any non-open barrier state is a hard stop for safety.
    state = str(barrier_state or "unknown").lower()
    if state == "open":
        return "advance"
    return "stop"


class EmergencyPriorityPolicy:
    @staticmethod
    def resolve(normal_request_active: bool, emergency_active: bool) -> bool:
        # Emergency always has priority in this MVP.
        if emergency_active:
            return True
        return normal_request_active


class Coordinator:
    def __init__(self, traffic_light_service, barrier_service=None):
        self.traffic_light_service = traffic_light_service
        self.barrier_service = barrier_service

    def handle_event(self, emergency_active: bool, normal_request_active: bool = False) -> CoordinatorResult:
        priority = EmergencyPriorityPolicy.resolve(normal_request_active, emergency_active)

        if priority:
            light = self.traffic_light_service.handle_request({"action": "emergency_on"})
            barrier_state = None
            if self.barrier_service is not None:
                barrier = self.barrier_service.handle_request_via_backend({"action": "open"})
                barrier_state = barrier.get("state")
            return CoordinatorResult(True, str(light.get("state", "unknown")), barrier_state)

        light = self.traffic_light_service.handle_request({"action": "emergency_off"})
        barrier_state = None
        if self.barrier_service is not None:
            barrier = self.barrier_service.handle_request_via_backend({"action": "close"})
            barrier_state = barrier.get("state")
        return CoordinatorResult(False, str(light.get("state", "unknown")), barrier_state)

    def resolve_road_scenario(self, data: RoadScenarioInput) -> RoadScenarioResult:
        """Resolve full roadside behavior for normal and emergency vehicles.

        Rules:
        - NORMAL mode: combine traffic-light and barrier rules safely.
        - EMERGENCY mode + approaching + same_lane: nearest light blinks all,
          other lights forced red, barrier opens, street lights blink, siren on.
        """
        # Scenario 2: emergency vehicle approaching in same lane.
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

        # Scenario 1: normal behavior (or emergency vehicle outside approach zone).
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
