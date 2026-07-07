from __future__ import annotations

from trafficlight_simulator import TrafficLightSimulator


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


def test_emergency_on_sets_emergency_green():
    sim = TrafficLightSimulator(make_config())
    response = sim.handle_request({"action": "emergency_on"})
    assert response["state"] == "emergency_green"


def test_yellow_action_sets_yellow():
    sim = TrafficLightSimulator(make_config())
    response = sim.handle_request({"action": "yellow"})
    assert response["state"] == "yellow"


def test_green_action_sets_green():
    sim = TrafficLightSimulator(make_config())
    response = sim.handle_request({"action": "green"})
    assert response["state"] == "green"
