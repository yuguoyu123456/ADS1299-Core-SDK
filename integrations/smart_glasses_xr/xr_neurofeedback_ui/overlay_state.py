#!/usr/bin/env python3
"""Small vendor-neutral XR overlay state/staleness helper."""

from __future__ import annotations

from dataclasses import dataclass
import math
import time
from typing import Mapping


@dataclass(frozen=True)
class OverlaySnapshot:
    eeg_sequence: int
    eeg_timestamp_us: int
    signal_valid: bool
    metrics: dict[str, float]
    label: str | None
    received_monotonic_ns: int


class OverlayStateStore:
    def __init__(self, max_age_ns: int = 500_000_000) -> None:
        if max_age_ns <= 0:
            raise ValueError("max_age_ns must be positive")
        self.max_age_ns = max_age_ns
        self._snapshot: OverlaySnapshot | None = None

    def update(
        self,
        eeg_sequence: int,
        eeg_timestamp_us: int,
        signal_valid: bool,
        metrics: Mapping[str, float],
        label: str | None = None,
        *,
        received_monotonic_ns: int | None = None,
    ) -> OverlaySnapshot:
        if not 0 <= eeg_sequence <= 0xFFFFFFFF:
            raise ValueError("eeg_sequence must fit uint32")
        if not 0 <= eeg_timestamp_us <= 0xFFFFFFFF:
            raise ValueError("eeg_timestamp_us must fit uint32")

        clean: dict[str, float] = {}
        for key, value in metrics.items():
            if not key:
                raise ValueError("metric names must not be empty")
            numeric = float(value)
            if not math.isfinite(numeric):
                raise ValueError(f"metric {key!r} must be finite")
            clean[key] = numeric

        now = time.monotonic_ns() if received_monotonic_ns is None else received_monotonic_ns
        if now < 0:
            raise ValueError("received_monotonic_ns must be non-negative")

        self._snapshot = OverlaySnapshot(
            eeg_sequence=eeg_sequence,
            eeg_timestamp_us=eeg_timestamp_us,
            signal_valid=bool(signal_valid),
            metrics=clean,
            label=label,
            received_monotonic_ns=now,
        )
        return self._snapshot

    def current(self, *, now_monotonic_ns: int | None = None) -> OverlaySnapshot | None:
        if self._snapshot is None:
            return None
        now = time.monotonic_ns() if now_monotonic_ns is None else now_monotonic_ns
        age = now - self._snapshot.received_monotonic_ns
        if age < 0 or age > self.max_age_ns:
            return None
        return self._snapshot

    def clear(self) -> None:
        self._snapshot = None
