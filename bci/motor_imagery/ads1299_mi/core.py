"""Shared conventions for motor-imagery EEG algorithms."""

from __future__ import annotations

from dataclasses import dataclass
from collections.abc import Sequence
import numpy as np


def validate_fs(fs: float) -> float:
    fs = float(fs)
    if not np.isfinite(fs) or fs <= 0:
        raise ValueError("fs must be positive and finite")
    return fs


def as_epochs(epochs: np.ndarray, *, min_trials: int = 1, min_samples: int = 16) -> np.ndarray:
    """Return epochs as finite float trials x samples x channels."""
    x = np.asarray(epochs, dtype=float)
    if x.ndim == 2:
        x = x[None, :, :]
    if x.ndim != 3:
        raise ValueError("epochs must be trials x samples x channels")
    if x.shape[0] < min_trials or x.shape[1] < min_samples or x.shape[2] < 1:
        raise ValueError("epochs have too few trials/samples/channels")
    if not np.all(np.isfinite(x)):
        raise ValueError("epochs contain NaN or Inf")
    return x


def as_window(eeg: np.ndarray, *, min_samples: int = 16) -> np.ndarray:
    x = np.asarray(eeg, dtype=float)
    if x.ndim == 1:
        x = x[:, None]
    if x.ndim != 2 or x.shape[0] < min_samples or x.shape[1] < 1:
        raise ValueError("eeg must be samples x channels")
    if not np.all(np.isfinite(x)):
        raise ValueError("eeg contains NaN or Inf")
    return x


def validate_labels(labels: Sequence, n_trials: int, *, exactly_two: bool = False) -> np.ndarray:
    y = np.asarray(labels)
    if y.ndim != 1 or len(y) != int(n_trials):
        raise ValueError("labels must contain one value per trial")
    classes = np.unique(y)
    if exactly_two and len(classes) != 2:
        raise ValueError("this algorithm requires exactly two classes")
    if len(classes) < 2:
        raise ValueError("at least two classes are required")
    return y


@dataclass(frozen=True)
class MIDecision:
    label: object | None
    scores: dict[object, float]
    confidence: float
    rejected: bool = False
