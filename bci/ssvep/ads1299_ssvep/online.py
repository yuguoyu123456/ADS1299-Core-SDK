"""Small online window engine for SSVEP classifiers.

This module only handles sample buffering and decision cadence. It does not use
zero-phase filtering and does not pretend to solve transport latency, clock
synchronization or stimulus-marker alignment; those belong in the shared online
BCI layer later in the repository roadmap.
"""

from __future__ import annotations

from collections import deque
from dataclasses import dataclass
from typing import Protocol

import numpy as np

from .core import SSVEPDecision, as_eeg, validate_fs


class WindowClassifier(Protocol):
    def predict(self, eeg: np.ndarray) -> SSVEPDecision: ...


@dataclass(frozen=True)
class OnlineSSVEPDecision:
    end_sample: int
    end_time_s: float
    decision: SSVEPDecision


class SSVEPStreamClassifier:
    """Classify fixed rolling windows at a deterministic hop interval."""

    def __init__(
        self,
        classifier: WindowClassifier,
        fs: float,
        *,
        window_seconds: float = 2.0,
        hop_seconds: float = 0.25,
    ) -> None:
        self.classifier = classifier
        self.fs = validate_fs(fs)
        self.window_samples = int(round(float(window_seconds) * self.fs))
        self.hop_samples = int(round(float(hop_seconds) * self.fs))
        if self.window_samples < 8:
            raise ValueError("window_seconds is too short")
        if self.hop_samples < 1:
            raise ValueError("hop_seconds is too short")
        self._rows: deque[np.ndarray] = deque(maxlen=self.window_samples)
        self._channels: int | None = None
        self.total_samples = 0
        self._next_emit = self.window_samples

    def reset(self) -> None:
        self._rows.clear()
        self._channels = None
        self.total_samples = 0
        self._next_emit = self.window_samples

    def push(self, samples: np.ndarray) -> list[OnlineSSVEPDecision]:
        """Push one or more samples and return every decision crossed by the chunk."""
        x = as_eeg(samples, min_samples=1)
        if self._channels is None:
            self._channels = x.shape[1]
        elif x.shape[1] != self._channels:
            raise ValueError("channel count changed within one stream")

        output: list[OnlineSSVEPDecision] = []
        for row in x:
            self._rows.append(np.asarray(row, dtype=float).copy())
            self.total_samples += 1
            if self.total_samples >= self._next_emit and len(self._rows) == self.window_samples:
                window = np.stack(tuple(self._rows), axis=0)
                result = self.classifier.predict(window)
                output.append(
                    OnlineSSVEPDecision(
                        end_sample=self.total_samples,
                        end_time_s=self.total_samples / self.fs,
                        decision=result,
                    )
                )
                self._next_emit += self.hop_samples
        return output
