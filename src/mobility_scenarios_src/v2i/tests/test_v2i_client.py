from __future__ import annotations

import time

from barrier_simulator import BarrierSimulator
from v2i_client import V2IClient


class DummyKuksa:
    def __init__(self):
        self.values = []

    def publish_status(self, value: str) -> None:
        self.values.append(value)


def make_config() -> dict:
    return {
        "mqtt": {"host": "127.0.0.1", "port": 1883, "keepalive": 60},
        "barrier": {"id": "b1", "open_duration_s": 1.0},
        "v2i": {
            "request_topic_template": "v2i/barrier/{id}/request",
            "status_topic_template": "v2i/barrier/{id}/status",
            "timeout_ms": 100,
            "retry_count": 0,
        },
        "vehicle": {
            "simulation_mode": True,
            "initial_distance_m": 10.0,
            "trigger_distance_m": 5.0,
            "approach_step_m": 1.0,
            "loop_interval_s": 0.01,
        },
        "kuksa": {
            "enabled": False,
            "host": "127.0.0.1",
            "port": 55555,
            "path_template": "Vehicle.Infra.Barrier.{id}.Status",
        },
    }


def test_should_request_open_when_close_enough():
    app = V2IClient(make_config())
    app.state.distance_m = 5.0
    app.state.request_sent_at = None
    assert app._should_request_open() is True


def test_timeout_sets_fail_safe_status_when_retries_exhausted():
    app = V2IClient(make_config())
    app.kuksa = DummyKuksa()
    app.state.request_sent_at = time.time() - 1.0
    app._handle_timeout()
    assert app.state.last_barrier_state == "timeout"
    assert app.kuksa.values[-1] == "timeout"


def test_local_barrier_flow_opens_and_returns_status():
    config = make_config()
    config["barrier"]["open_duration_s"] = 0.01

    barrier = BarrierSimulator(config)
    app = V2IClient(config, barrier_service=barrier)
    app.kuksa = DummyKuksa()

    app.state.distance_m = 5.0
    app._send_open_request()

    assert app.state.last_barrier_state == "open"
    assert app.kuksa.values[-1] == "open"
