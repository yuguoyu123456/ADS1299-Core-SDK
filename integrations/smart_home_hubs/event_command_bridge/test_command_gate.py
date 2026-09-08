#!/usr/bin/env python3
"""Smoke test for command_gate.py."""

from command_gate import CommandGate


def main() -> None:
    gate = CommandGate(
        {"scene_next", "scene_previous"},
        min_confidence=0.8,
        required_repeats=2,
        cooldown_ns=100,
    )

    d = gate.propose("unknown", 0.99, 1, now_monotonic_ns=1000)
    assert not d.accepted

    d = gate.propose("scene_next", 0.5, 2, now_monotonic_ns=1100)
    assert not d.accepted

    d = gate.propose("scene_next", 0.9, 3, now_monotonic_ns=1200)
    assert not d.accepted
    assert "1/2" in d.reason

    d = gate.propose("scene_next", 0.95, 4, now_monotonic_ns=1210)
    assert d.accepted

    d = gate.propose("scene_next", 0.95, 5, now_monotonic_ns=1250)
    assert not d.accepted
    assert "cooldown" in d.reason

    d = gate.propose("scene_next", 0.95, 6, signal_valid=False, now_monotonic_ns=1400)
    assert not d.accepted

    gate.set_enabled(False)
    d = gate.propose("scene_next", 0.99, 7, now_monotonic_ns=1500)
    assert not d.accepted
    assert "disabled" in d.reason

    gate.set_enabled(True)
    d = gate.propose("scene_previous", 0.9, 8, now_monotonic_ns=1600)
    assert not d.accepted
    d = gate.propose("scene_previous", 0.9, 9, now_monotonic_ns=1700)
    assert d.accepted

    print("smart-home command gate smoke test: PASS")


if __name__ == "__main__":
    main()
