from __future__ import annotations

from barrier_rules import decide_motion_from_barrier_state


def test_barrier_open_means_advance():
    assert decide_motion_from_barrier_state("open") == "advance"


def test_barrier_closed_means_stop():
    assert decide_motion_from_barrier_state("closed") == "stop"


def test_barrier_unknown_means_stop_for_safety():
    assert decide_motion_from_barrier_state("unknown") == "stop"
