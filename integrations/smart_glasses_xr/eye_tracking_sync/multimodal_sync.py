#!/usr/bin/env python3
"""Dependency-light helpers for aligning EEG and eye-tracking samples.

All matching functions operate on an already defined common monotonic timebase.
Clock mapping is explicit; arrival order is never used as synchronization.
"""

from __future__ import annotations

from bisect import bisect_left
from dataclasses import dataclass
from typing import Any, Sequence


@dataclass(frozen=True)
class LinearClockMapping:
    """Map source nanoseconds into a common nanosecond timebase.

    common_ns ~= offset_ns + scale * source_ns
    """

    offset_ns: float
    scale: float = 1.0

    def map_ns(self, source_ns: int) -> int:
        if source_ns < 0:
            raise ValueError("source_ns must be non-negative")
        if self.scale <= 0.0:
            raise ValueError("scale must be positive")
        return int(round(self.offset_ns + self.scale * float(source_ns)))


@dataclass(frozen=True)
class EyeSample:
    common_time_ns: int
    valid: bool
    payload: Any


@dataclass(frozen=True)
class AlignmentResult:
    eeg_common_time_ns: int
    eye_sample: EyeSample | None
    delta_ns: int | None


def nearest_eye_sample(
    eeg_common_time_ns: int,
    eye_samples: Sequence[EyeSample],
    max_delta_ns: int,
) -> AlignmentResult:
    if eeg_common_time_ns < 0:
        raise ValueError("eeg_common_time_ns must be non-negative")
    if max_delta_ns < 0:
        raise ValueError("max_delta_ns must be non-negative")
    if not eye_samples:
        return AlignmentResult(eeg_common_time_ns, None, None)

    times = [sample.common_time_ns for sample in eye_samples]
    if any(times[i] > times[i + 1] for i in range(len(times) - 1)):
        raise ValueError("eye_samples must be sorted by common_time_ns")

    index = bisect_left(times, eeg_common_time_ns)
    candidates: list[EyeSample] = []
    if index < len(eye_samples):
        candidates.append(eye_samples[index])
    if index > 0:
        candidates.append(eye_samples[index - 1])

    candidates = [sample for sample in candidates if sample.valid]
    if not candidates:
        return AlignmentResult(eeg_common_time_ns, None, None)

    best = min(candidates, key=lambda sample: abs(sample.common_time_ns - eeg_common_time_ns))
    delta = best.common_time_ns - eeg_common_time_ns
    if abs(delta) > max_delta_ns:
        return AlignmentResult(eeg_common_time_ns, None, None)

    return AlignmentResult(eeg_common_time_ns, best, delta)
