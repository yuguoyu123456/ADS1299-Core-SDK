#!/usr/bin/env python3
"""Dependency-light gate for converting BCI proposals into command intents.

This module does not execute smart-home actions. It only decides whether a
proposal is eligible to be handed to a separate automation adapter.
"""

from __future__ import annotations

from dataclasses import dataclass
import math
import time
from typing import Iterable


@dataclass(frozen=True)
class GateDecision:
    accepted: bool
    command: str
    source_sequence: int
    confidence: float
    host_monotonic_ns: int
    reason: str


class CommandGate:
    def __init__(
        self,
        allowed_commands: Iterable[str],
        *,
        min_confidence: float = 0.8,
        required_repeats: int = 2,
        cooldown_ns: int = 1_000_000_000,
    ) -> None:
        allowed = {item for item in allowed_commands if item}
        if not allowed:
            raise ValueError("allowed_commands must not be empty")
        if not 0.0 <= min_confidence <= 1.0:
            raise ValueError("min_confidence must be in [0, 1]")
        if required_repeats <= 0:
            raise ValueError("required_repeats must be positive")
        if cooldown_ns < 0:
            raise ValueError("cooldown_ns must be non-negative")

        self.allowed_commands = allowed
        self.min_confidence = min_confidence
        self.required_repeats = required_repeats
        self.cooldown_ns = cooldown_ns
        self.enabled = True
        self._candidate: str | None = None
        self._candidate_repeats = 0
        self._last_accept_ns: int | None = None

    def set_enabled(self, enabled: bool) -> None:
        self.enabled = bool(enabled)
        if not self.enabled:
            self._candidate = None
            self._candidate_repeats = 0

    def reset(self) -> None:
        self._candidate = None
        self._candidate_repeats = 0
        self._last_accept_ns = None

    def propose(
        self,
        command: str,
        confidence: float,
        source_sequence: int,
        *,
        signal_valid: bool = True,
        now_monotonic_ns: int | None = None,
    ) -> GateDecision:
        now = time.monotonic_ns() if now_monotonic_ns is None else now_monotonic_ns
        if now < 0:
            raise ValueError("now_monotonic_ns must be non-negative")
        if not 0 <= source_sequence <= 0xFFFFFFFF:
            raise ValueError("source_sequence must fit uint32")

        value = float(confidence)
        if not math.isfinite(value):
            value = -1.0

        def reject(reason: str, clear_candidate: bool = True) -> GateDecision:
            if clear_candidate:
                self._candidate = None
                self._candidate_repeats = 0
            return GateDecision(False, command, source_sequence, value, now, reason)

        if not self.enabled:
            return reject("gate disabled by user/system override")
        if command not in self.allowed_commands:
            return reject("command not in allowlist")
        if not signal_valid:
            return reject("signal invalid")
        if not 0.0 <= value <= 1.0 or value < self.min_confidence:
            return reject("confidence below threshold or invalid")
        if self._last_accept_ns is not None and now - self._last_accept_ns < self.cooldown_ns:
            return reject("cooldown active")

        if command == self._candidate:
            self._candidate_repeats += 1
        else:
            self._candidate = command
            self._candidate_repeats = 1

        if self._candidate_repeats < self.required_repeats:
            return GateDecision(
                False,
                command,
                source_sequence,
                value,
                now,
                f"waiting for repeat {self._candidate_repeats}/{self.required_repeats}",
            )

        self._candidate = None
        self._candidate_repeats = 0
        self._last_accept_ns = now
        return GateDecision(True, command, source_sequence, value, now, "command intent accepted")
