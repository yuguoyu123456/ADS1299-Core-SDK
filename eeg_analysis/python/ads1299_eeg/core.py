"""Shared validation helpers for ADS1299 EEG analysis.

Conventions
-----------
Continuous data are represented as ``samples x channels``.
Epoched data are represented as ``epochs x samples x channels``.
Unless documented otherwise, host-side EEG amplitudes are expected in microvolts.
"""

from __future__ import annotations

from collections.abc import Sequence

import numpy as np


DEFAULT_EEG_BANDS: dict[str, tuple[float, float]] = {
    "delta": (0.5, 4.0),
    "theta": (4.0, 8.0),
    "alpha": (8.0, 13.0),
    "beta": (13.0, 30.0),
    "gamma": (30.0, 45.0),
}


def as_continuous(data: np.ndarray, *, copy: bool = False) -> tuple[np.ndarray, bool]:
    """Return a floating-point ``samples x channels`` array.

    Parameters
    ----------
    data:
        One-dimensional single-channel data or a two-dimensional continuous
        array shaped ``samples x channels``.
    copy:
        Force a writable copy.

    Returns
    -------
    array, was_1d
        ``was_1d`` allows callers to restore a single-channel output shape.
    """
    x = np.asarray(data, dtype=float)
    was_1d = x.ndim == 1
    if was_1d:
        x = x[:, None]
    elif x.ndim != 2:
        raise ValueError("continuous EEG must be 1-D or samples x channels")
    if x.shape[0] < 2:
        raise ValueError("continuous EEG must contain at least two samples")
    if x.shape[1] < 1:
        raise ValueError("continuous EEG must contain at least one channel")
    return (x.copy() if copy else x), was_1d


def as_epochs(epochs: np.ndarray, *, copy: bool = False) -> np.ndarray:
    """Validate an ``epochs x samples x channels`` array."""
    x = np.asarray(epochs, dtype=float)
    if x.ndim != 3:
        raise ValueError("epochs must be epochs x samples x channels")
    if x.shape[0] < 1 or x.shape[1] < 2 or x.shape[2] < 1:
        raise ValueError("epochs must contain data")
    return x.copy() if copy else x


def restore_channel_shape(data: np.ndarray, was_1d: bool) -> np.ndarray:
    """Restore a one-dimensional result when the input was one-dimensional."""
    return data[:, 0] if was_1d else data


def validate_sampling_rate(fs: float) -> float:
    """Return a positive finite sampling rate as float."""
    fs = float(fs)
    if not np.isfinite(fs) or fs <= 0:
        raise ValueError("sampling rate must be a positive finite value")
    return fs


def validate_frequency_range(
    low: float,
    high: float,
    fs: float,
    *,
    allow_zero_low: bool = False,
) -> tuple[float, float]:
    """Validate a frequency interval against Nyquist."""
    fs = validate_sampling_rate(fs)
    low = float(low)
    high = float(high)
    lower_ok = low >= 0 if allow_zero_low else low > 0
    if not lower_ok or not np.isfinite(low) or not np.isfinite(high):
        raise ValueError("frequency limits must be finite")
    if high <= low:
        raise ValueError("high frequency must be greater than low frequency")
    if high >= fs / 2:
        raise ValueError("high frequency must be below Nyquist")
    return low, high


def normalize_channel_indices(indices: Sequence[int], n_channels: int) -> np.ndarray:
    """Validate channel indices and return a unique integer array."""
    idx = np.asarray(list(indices), dtype=int)
    if idx.size == 0:
        raise ValueError("at least one channel index is required")
    if np.any(idx < 0) or np.any(idx >= n_channels):
        raise IndexError("channel index out of range")
    return np.unique(idx)


def default_channel_names(n_channels: int, prefix: str = "CH") -> list[str]:
    """Return readable 1-based channel names such as CH1..CH8."""
    if n_channels < 1:
        raise ValueError("n_channels must be positive")
    return [f"{prefix}{index + 1}" for index in range(n_channels)]
