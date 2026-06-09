from __future__ import annotations

from coordinator import Coordinator, EmergencyPriorityPolicy
from trafficlight_simulator import TrafficLightSimulator
from barrier_simulator import BarrierSimulator


def make_config() -> dict:
    return {
        "traffic_light": {"id": "tl1", "backend": "simulation", "emergency_green_duration_s": 0.01},
        "emergency": {"timeout_ms": 100, "retry_count": 0, "source": "ambulance"},
        "vehicle": {"simulation_mode": True, "trigger_after_s": 0.01, "loop_interval_s": 0.01},
        "kuksa": {
            "enabled": False,
            "priority_path": "Vehicle.Emergency.Priority.Active",
            "traffic_light_path_template": "Vehicle.Infra.TrafficLight.{id}.Status",
        },
    }


def test_policy_emergency_overrides_normal_flow():
    assert EmergencyPriorityPolicy.resolve(normal_request_active=True, emergency_active=True) is True


def test_coordinator_emergency_sets_green():
    service = TrafficLightSimulator(make_config())
    coord = Coordinator(service)
    result = coord.handle_event(emergency_active=True, normal_request_active=False)
    assert result.priority_active is True
    assert result.traffic_light_state == "emergency_green"


def test_coordinator_emergency_opens_barrier_when_connected():
    cfg = make_config()
    barrier_cfg = {
        "barrier": {"id": "b1", "backend": "simulation", "open_duration_s": 0.01},
        "v2i": {"timeout_ms": 100, "retry_count": 0},
        "vehicle": {
            "simulation_mode": True,
            "initial_distance_m": 10.0,
            "trigger_distance_m": 5.0,
            "approach_step_m": 1.0,
            "loop_interval_s": 0.01,
        },
        "kuksa": {
            "enabled": False,
            "path_template": "Vehicle.Infra.Barrier.{id}.Status",
        },
    }
    light = TrafficLightSimulator(cfg)
    barrier = BarrierSimulator(barrier_cfg)

    coord = Coordinator(light, barrier)
    result = coord.handle_event(emergency_active=True, normal_request_active=False)

    assert result.priority_active is True
    assert result.traffic_light_state == "emergency_green"
    assert result.barrier_state == "open"


def test_coordinator_non_emergency_closes_barrier_and_red_light():
    cfg = make_config()
    barrier_cfg = {
        "barrier": {"id": "b1", "backend": "simulation", "open_duration_s": 0.01},
        "v2i": {"timeout_ms": 100, "retry_count": 0},
        "vehicle": {
            "simulation_mode": True,
            "initial_distance_m": 10.0,
            "trigger_distance_m": 5.0,
            "approach_step_m": 1.0,
            "loop_interval_s": 0.01,
        },
        "kuksa": {
            "enabled": False,
            "path_template": "Vehicle.Infra.Barrier.{id}.Status",
        },
    }
    light = TrafficLightSimulator(cfg)
    barrier = BarrierSimulator(barrier_cfg)
    coord = Coordinator(light, barrier)

    # Open first with emergency, then clear emergency and verify fallback behavior.
    _ = coord.handle_event(emergency_active=True, normal_request_active=False)
    result = coord.handle_event(emergency_active=False, normal_request_active=False)

    assert result.priority_active is False
    assert result.traffic_light_state == "red"
    assert result.barrier_state == "closed"
