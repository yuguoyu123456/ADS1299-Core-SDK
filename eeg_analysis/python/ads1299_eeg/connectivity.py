"""Starter functional-connectivity measures for EEG."""

from __future__ import annotations

import numpy as np
from scipy import signal

from .core import as_continuous, validate_frequency_range, validate_sampling_rate


def correlation_matrix(data: np.ndarray) -> np.ndarray:
    """Return channel-by-channel Pearson correlation matrix."""
    x, _ = as_continuous(data)
    if np.isnan(x).any():
        raise ValueError("correlation_matrix does not accept NaN values")
    return np.corrcoef(x, rowvar=False)


def coherence_matrix(
    data: np.ndarray,
    fs: float,
    *,
    band: tuple[float, float] = (8.0, 13.0),
    nperseg: int | None = None,
) -> np.ndarray:
    """Average magnitude-squared coherence within a frequency band."""
    fs = validate_sampling_rate(fs)
    low, high = validate_frequency_range(band[0], band[1], fs)
    x, _ = as_continuous(data)
    if np.isnan(x).any():
        raise ValueError("coherence_matrix does not accept NaN values")
    channels = x.shape[1]
    result = np.eye(channels, dtype=float)
    if nperseg is None:
        nperseg = min(x.shape[0], max(64, int(round(2 * fs))))
    for i in range(channels):
        for j in range(i + 1, channels):
            frequencies, coherence = signal.coherence(
                x[:, i], x[:, j], fs=fs, nperseg=min(int(nperseg), x.shape[0])
            )
            mask = (frequencies >= low) & (frequencies <= high)
            value = float(np.mean(coherence[mask])) if np.any(mask) else float("nan")
            result[i, j] = value
            result[j, i] = value
    return result


def phase_locking_value(
    data: np.ndarray,
    fs: float,
    *,
    band: tuple[float, float] = (8.0, 13.0),
    filter_order: int = 4,
) -> np.ndarray:
    """Compute channel-by-channel phase-locking value (PLV).

    Signals are band-pass filtered with a zero-phase Butterworth filter and
    analytic phase is obtained with the Hilbert transform.
    """
    fs = validate_sampling_rate(fs)
    low, high = validate_frequency_range(band[0], band[1], fs)
    x, _ = as_continuous(data)
    if np.isnan(x).any():
        raise ValueError("phase_locking_value does not accept NaN values")
    sos = signal.butter(filter_order, [low, high], btype="bandpass", fs=fs, output="sos")
    filtered = signal.sosfiltfilt(sos, x, axis=0)
    phase = np.angle(signal.hilbert(filtered, axis=0))
    channels = x.shape[1]
    result = np.eye(channels, dtype=float)
    for i in range(channels):
        for j in range(i + 1, channels):
            delta = phase[:, i] - phase[:, j]
            value = float(np.abs(np.mean(np.exp(1j * delta))))
            result[i, j] = value
            result[j, i] = value
    return result
