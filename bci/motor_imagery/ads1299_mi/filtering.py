"""Filtering helpers for offline motor-imagery epochs."""

from __future__ import annotations

import numpy as np
from scipy import signal

from .core import as_epochs, validate_fs


def bandpass_epochs(
    epochs: np.ndarray,
    fs: float,
    band: tuple[float, float] = (8.0, 30.0),
    *,
    order: int = 4,
) -> np.ndarray:
    """Zero-phase Butterworth band-pass over the sample axis.

    This function is intended for offline calibration/evaluation. Real-time
    feedback should use a causal stateful filter in the shared online layer.
    """
    x = as_epochs(epochs)
    fs = validate_fs(fs)
    low, high = map(float, band)
    if not 0 < low < high < fs / 2:
        raise ValueError("band must satisfy 0 < low < high < Nyquist")
    if int(order) < 1:
        raise ValueError("order must be positive")
    sos = signal.butter(int(order), [low, high], btype="bandpass", fs=fs, output="sos")
    try:
        return signal.sosfiltfilt(sos, x, axis=1)
    except ValueError as exc:
        raise ValueError("epochs are too short for this zero-phase filter") from exc


def notch_epochs(
    epochs: np.ndarray,
    fs: float,
    mains_hz: float = 50.0,
    *,
    q: float = 30.0,
) -> np.ndarray:
    """Offline zero-phase IIR notch over the sample axis."""
    x = as_epochs(epochs)
    fs = validate_fs(fs)
    mains = float(mains_hz)
    if not 0 < mains < fs / 2:
        raise ValueError("mains_hz must lie below Nyquist")
    if not np.isfinite(q) or q <= 0:
        raise ValueError("q must be positive")
    b, a = signal.iirnotch(mains, float(q), fs=fs)
    try:
        return signal.filtfilt(b, a, x, axis=1)
    except ValueError as exc:
        raise ValueError("epochs are too short for this zero-phase notch") from exc
