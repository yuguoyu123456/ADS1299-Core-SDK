"""P300/ERP beginner utilities.

Educational/reference implementation; not a complete speller pipeline.
Epoch convention: trials x samples x channels.
"""

from __future__ import annotations
import numpy as np


def baseline_correct(epochs: np.ndarray, times_s: np.ndarray, baseline=(-0.2, 0.0)) -> np.ndarray:
    epochs = np.asarray(epochs, dtype=float)
    times_s = np.asarray(times_s, dtype=float)
    mask = (times_s >= baseline[0]) & (times_s <= baseline[1])
    if not np.any(mask):
        raise ValueError("Baseline interval has no samples")
    base = np.mean(epochs[:, mask, :], axis=1, keepdims=True)
    return epochs - base


def grand_average(epochs: np.ndarray) -> np.ndarray:
    return np.mean(np.asarray(epochs, dtype=float), axis=0)


def p300_amplitude(erp: np.ndarray, times_s: np.ndarray, window=(0.25, 0.50)) -> np.ndarray:
    erp = np.asarray(erp, dtype=float)
    times_s = np.asarray(times_s, dtype=float)
    mask = (times_s >= window[0]) & (times_s <= window[1])
    if not np.any(mask):
        raise ValueError("P300 window has no samples")
    return np.mean(erp[mask, :], axis=0)


def p300_peak_latency(erp: np.ndarray, times_s: np.ndarray, window=(0.25, 0.50)) -> np.ndarray:
    erp = np.asarray(erp, dtype=float)
    times_s = np.asarray(times_s, dtype=float)
    mask = (times_s >= window[0]) & (times_s <= window[1])
    local_t = times_s[mask]
    local = erp[mask, :]
    return local_t[np.argmax(local, axis=0)]
