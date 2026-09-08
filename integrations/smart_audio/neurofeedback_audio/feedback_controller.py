#!/usr/bin/env python3
"""Small dependency-light research feedback controller.

The controller maps a normalized finite research score into an abstract
0..1 feedback level. It does not control device volume directly and does not
encode any therapeutic or clinical interpretation.
"""

from __future__ import annotations

from dataclasses import dataclass
import math
import time


@dataclass(frozen=True)
class FeedbackUpdate:
    input_score: float | None
    output_level: float
    valid: bool
    host_monotonic_ns: int
    reason: str


class FeedbackController:
    def __init__(self, smoothing_alpha: float = 0.25, neutral_level: float = 0.5) -> None:
        if not 0.0 < smoothing_alpha <= 1.0:
            raise ValueError("smoothing_alpha must be in (0, 1]")
        if not 0.0 <= neutral_level <= 1.0:
            raise ValueError("neutral_level must be in [0, 1]")
        self.smoothing_alpha = smoothing_alpha
        self.neutral_level = neutral_level
        self._state = neutral_level

    @property
    def output_level(self) -> float:
        return self._state

    def reset(self) -> None:
        self._state = self.neutral_level

    def update(self, score: float | None, *, signal_valid: bool = True) -> FeedbackUpdate:
        now = time.monotonic_ns()

        if score is None or not signal_valid:
            self._state = self.neutral_level
            return FeedbackUpdate(
                input_score=score,
                output_level=self._state,
                valid=False,
                host_monotonic_ns=now,
                reason="invalid or unavailable signal -> neutral feedback",
            )

        value = float(score)
        if not math.isfinite(value):
            self._state = self.neutral_level
            return FeedbackUpdate(
                input_score=value,
                output_level=self._state,
                valid=False,
                host_monotonic_ns=now,
                reason="non-finite score -> neutral feedback",
            )

        value = min(1.0, max(0.0, value))
        self._state = (
            self.smoothing_alpha * value
            + (1.0 - self.smoothing_alpha) * self._state
        )
        self._state = min(1.0, max(0.0, self._state))
        return FeedbackUpdate(
            input_score=value,
            output_level=self._state,
            valid=True,
            host_monotonic_ns=now,
            reason="normalized score accepted",
        )
