from __future__ import annotations

from traffic_light_rules import decide_motion_from_traffic_light_state


def test_green_means_advance():
    assert decide_motion_from_traffic_light_state("green") == "advance"


def test_emergency_green_means_advance():
    assert decide_motion_from_traffic_light_state("emergency_green") == "advance"


def test_yellow_means_slow_down():
    assert decide_motion_from_traffic_light_state("yellow") == "slow_down"


def test_red_means_stop():
    assert decide_motion_from_traffic_light_state("red") == "stop"


def test_unknown_means_stop_for_safety():
    assert decide_motion_from_traffic_light_state("unknown") == "stop"
