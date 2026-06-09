#!/usr/bin/env python3
"""Unified coordinator scaffold for Phase 3.

For now it focuses on emergency override policy and local orchestration hooks.
"""

from __future__ import annotations

from dataclasses import dataclass
from typing import Optional


@dataclass
class CoordinatorResult:
    priority_active: bool
    traffic_light_state: str
    barrier_state: Optional[str] = None


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
