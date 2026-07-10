from __future__ import annotations

import time

from emergency_client import EmergencyClient
from trafficlight_simulator import TrafficLightSimulator


class DummyKuksa:
    def __init__(self):
        self.records = []

    def publish(self, priority_active: bool, light_state: str) -> None:
        self.records.append((priority_active, light_state))


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


def test_emergency_request_activates_priority():
    config = make_config()
    service = TrafficLightSimulator(config)
    client = EmergencyClient(config, traffic_light_service=service)
    client.kuksa = DummyKuksa()

    client._send_priority_request()

    assert client.state.priority_active is True
    assert client.state.last_light_state == "emergency_green"
    assert client.kuksa.records[-1] == (True, "emergency_green")


def test_timeout_sets_timeout_status_when_no_service():
    config = make_config()
    client = EmergencyClient(config, traffic_light_service=None)
    client.kuksa = DummyKuksa()
    client.state.request_sent_at = time.time() - 1.0

    client._handle_timeout()

    assert client.state.priority_active is False
    assert client.state.last_light_state == "timeout"
    assert client.kuksa.records[-1] == (False, "timeout")
