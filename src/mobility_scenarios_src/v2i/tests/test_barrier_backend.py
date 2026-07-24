from __future__ import annotations

from barrier_backend import is_hardware_backend
from kitronik_barrier import KitronikBarrierAdapter


def test_backend_defaults_to_simulation():
    assert is_hardware_backend({"barrier": {}}) is False


def test_backend_detects_hardware_mode():
    assert is_hardware_backend({"barrier": {"backend": "hardware"}}) is True


def test_kitronik_placeholder_returns_status():
    adapter = KitronikBarrierAdapter("b1")
    response = adapter.handle_request({"action": "open"})
    assert response["barrier_id"] == "b1"
    assert response["state"] == "open"
    assert response["backend"] == "kitronik-placeholder"
