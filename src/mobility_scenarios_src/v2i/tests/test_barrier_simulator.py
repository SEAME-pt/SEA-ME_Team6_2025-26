from __future__ import annotations

from barrier_simulator import build_topic


def test_build_topic_replaces_barrier_id():
    topic = build_topic("v2i/barrier/{id}/request", "kitronik_demo_1")
    assert topic == "v2i/barrier/kitronik_demo_1/request"
