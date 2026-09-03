"""ERD/ERS time-course helper for motor-imagery EEG.

Epoch convention: trials x samples x channels. Output is percentage change in
analytic band power relative to a baseline interval. Negative values indicate
ERD; positive values indicate ERS relative to that baseline.
"""

from __future__ import annotations
import numpy as np
from scipy import signal


def erd_ers(
    epochs: np.ndarray,
    fs: float,
    times_s: np.ndarray,
    band=(8.0, 13.0),
    baseline=(-1.5, -0.5),
    smooth_ms: float = 200.0,
):
    ep = np.asarray(epochs, dtype=float)
    times = np.asarray(times_s, dtype=float)
    if ep.ndim != 3:
        raise ValueError("epochs must be trials x samples x channels")
    if ep.shape[1] != len(times):
        raise ValueError("times length must match epoch samples")
    low, high = map(float, band)
    if not 0 < low < high < fs / 2:
        raise ValueError("band must be below Nyquist")

    sos = signal.butter(4, [low, high], btype="bandpass", fs=fs, output="sos")
    filtered = signal.sosfiltfilt(sos, ep, axis=1)
    analytic = signal.hilbert(filtered, axis=1)
    power = np.abs(analytic) ** 2

    if smooth_ms > 0:
        n = max(1, int(round(smooth_ms * 1e-3 * fs)))
        kernel = np.ones(n) / n
        power = signal.convolve(power, kernel[None, :, None], mode="same")

    bmask = (times >= baseline[0]) & (times <= baseline[1])
    if not np.any(bmask):
        raise ValueError("baseline interval has no samples")
    base = np.mean(power[:, bmask, :], axis=1, keepdims=True)
    percent = 100.0 * (power - base) / np.maximum(base, np.finfo(float).eps)
    return percent, np.mean(percent, axis=0)
