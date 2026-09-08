#!/usr/bin/env python3
"""Smoke test for multimodal_sync.py."""

from multimodal_sync import EyeSample, LinearClockMapping, nearest_eye_sample


def main() -> None:
    mapping = LinearClockMapping(offset_ns=1000.0, scale=1.000001)
    mapped = mapping.map_ns(1_000_000)
    assert mapped > 1_000_000

    samples = [
        EyeSample(common_time_ns=1_000, valid=True, payload={"x": 0.1}),
        EyeSample(common_time_ns=2_000, valid=False, payload={"x": 0.2}),
        EyeSample(common_time_ns=3_000, valid=True, payload={"x": 0.3}),
    ]

    result = nearest_eye_sample(1_100, samples, max_delta_ns=200)
    assert result.eye_sample is not None
    assert result.eye_sample.payload["x"] == 0.1
    assert result.delta_ns == -100

    # Nearest timestamp is invalid, so the valid sample outside tolerance must not be reused.
    result = nearest_eye_sample(2_000, samples, max_delta_ns=200)
    assert result.eye_sample is None

    # A valid sample exists, but is too far away.
    result = nearest_eye_sample(2_500, samples, max_delta_ns=100)
    assert result.eye_sample is None

    try:
        nearest_eye_sample(
            1_000,
            [
                EyeSample(2_000, True, None),
                EyeSample(1_000, True, None),
            ],
            100,
        )
        raise AssertionError("unsorted eye samples accepted")
    except ValueError:
        pass

    print("EEG eye-tracking sync smoke test: PASS")


if __name__ == "__main__":
    main()
