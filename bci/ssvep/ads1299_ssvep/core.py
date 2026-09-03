"""Shared validation and decision helpers for SSVEP algorithms."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Mapping, Sequence

import numpy as np


Array = np.ndarray


def as_eeg(eeg: Array, *, min_samples: int = 8) -> Array:
    """Return finite EEG as a float samples x channels matrix."""
    x = np.asarray(eeg, dtype=float)
    if x.ndim == 1:
        x = x[:, None]
    if x.ndim != 2:
        raise ValueError("eeg must be samples x channels")
    if x.shape[0] < min_samples or x.shape[1] < 1:
        raise ValueError("eeg has too few samples or no channels")
    if not np.all(np.isfinite(x)):
        raise ValueError("eeg contains NaN or Inf")
    return x


def validate_fs(fs: float) -> float:
    fs = float(fs)
    if not np.isfinite(fs) or fs <= 0:
        raise ValueError("fs must be positive and finite")
    return fs


def normalize_frequencies(frequencies: Sequence[float], fs: float) -> tuple[float, ...]:
    fs = validate_fs(fs)
    values = tuple(float(f) for f in frequencies)
    if len(values) < 2:
        raise ValueError("at least two target frequencies are required")
    if len(set(values)) != len(values):
        raise ValueError("target frequencies must be unique")
    if any((not np.isfinite(f)) or f <= 0 or f >= fs / 2 for f in values):
        raise ValueError("every target frequency must lie inside (0, Nyquist)")
    return values


def standardize_columns(x: Array) -> Array:
    x = np.asarray(x, dtype=float)
    x = x - np.mean(x, axis=0, keepdims=True)
    scale = np.std(x, axis=0, ddof=1, keepdims=True)
    scale[~np.isfinite(scale) | (scale <= np.finfo(float).eps)] = 1.0
    return x / scale


@dataclass(frozen=True)
class SSVEPDecision:
    """One SSVEP classification result."""

    label_hz: float | None
    scores: dict[float, float]
    best_score: float
    margin: float
    rejected: bool


def decision_from_scores(
    scores: Mapping[float, float],
    *,
    min_score: float | None = None,
    min_margin: float | None = None,
) -> SSVEPDecision:
    """Convert a score mapping into a ranked decision with optional rejection."""
    if not scores:
        raise ValueError("scores is empty")
    clean = {float(k): float(v) for k, v in scores.items()}
    if not all(np.isfinite(v) for v in clean.values()):
        raise ValueError("scores contain non-finite values")
    ranked = sorted(clean.items(), key=lambda kv: kv[1], reverse=True)
    best_label, best_score = ranked[0]
    second = ranked[1][1] if len(ranked) > 1 else -np.inf
    margin = best_score - second
    rejected = False
    if min_score is not None and best_score < float(min_score):
        rejected = True
    if min_margin is not None and margin < float(min_margin):
        rejected = True
    return SSVEPDecision(
        label_hz=None if rejected else best_label,
        scores=clean,
        best_score=best_score,
        margin=float(margin),
        rejected=rejected,
    )
