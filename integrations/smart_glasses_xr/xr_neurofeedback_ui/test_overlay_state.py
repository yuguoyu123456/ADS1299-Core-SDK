#!/usr/bin/env python3
"""Smoke test for overlay_state.py."""

from overlay_state import OverlayStateStore


def main() -> None:
    store = OverlayStateStore(max_age_ns=100)
    assert store.current(now_monotonic_ns=0) is None

    snapshot = store.update(
        42,
        123456,
        True,
        {"alpha": 0.4, "score": 0.8},
        label="target",
        received_monotonic_ns=1_000,
    )
    assert snapshot.eeg_sequence == 42
    assert snapshot.signal_valid

    current = store.current(now_monotonic_ns=1_050)
    assert current is not None
    assert current.metrics["score"] == 0.8

    stale = store.current(now_monotonic_ns=1_101)
    assert stale is None

    try:
        store.update(0, 0, True, {"bad": float("nan")})
        raise AssertionError("non-finite metric accepted")
    except ValueError:
        pass

    store.clear()
    assert store.current(now_monotonic_ns=1_050) is None

    print("XR overlay state smoke test: PASS")


if __name__ == "__main__":
    main()
