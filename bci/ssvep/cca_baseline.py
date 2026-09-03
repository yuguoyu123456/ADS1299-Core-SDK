"""Canonical-correlation SSVEP baseline.

Educational/reference implementation for ADS1299-Core-SDK.
Input EEG shape: samples x channels.
Requires NumPy only.
"""

from __future__ import annotations
import numpy as np


def _standardize(x: np.ndarray) -> np.ndarray:
    x = np.asarray(x, dtype=float)
    x = x - np.mean(x, axis=0, keepdims=True)
    scale = np.std(x, axis=0, ddof=1, keepdims=True)
    scale[scale == 0] = 1.0
    return x / scale


def canonical_corr(x: np.ndarray, y: np.ndarray, ridge: float = 1e-8) -> float:
    """Return largest canonical correlation using whitened cross-covariance."""
    x = _standardize(x)
    y = _standardize(y)
    n = min(len(x), len(y))
    if n < 4:
        raise ValueError("Need at least four samples")
    x = x[:n]
    y = y[:n]
    cxx = (x.T @ x) / (n - 1) + ridge * np.eye(x.shape[1])
    cyy = (y.T @ y) / (n - 1) + ridge * np.eye(y.shape[1])
    cxy = (x.T @ y) / (n - 1)

    def invsqrt(c):
        d, v = np.linalg.eigh(c)
        d = np.maximum(d, ridge)
        return (v * (1.0 / np.sqrt(d))) @ v.T

    m = invsqrt(cxx) @ cxy @ invsqrt(cyy)
    s = np.linalg.svd(m, compute_uv=False)
    return float(np.clip(s[0], 0.0, 1.0))


def ssvep_references(freq_hz: float, fs: float, n_samples: int, harmonics: int = 3) -> np.ndarray:
    t = np.arange(n_samples) / fs
    cols = []
    for h in range(1, harmonics + 1):
        cols.append(np.sin(2 * np.pi * h * freq_hz * t))
        cols.append(np.cos(2 * np.pi * h * freq_hz * t))
    return np.column_stack(cols)


def predict_ssvep(eeg: np.ndarray, fs: float, frequencies, harmonics: int = 3):
    eeg = np.asarray(eeg, dtype=float)
    if eeg.ndim == 1:
        eeg = eeg[:, None]
    scores = {}
    for f in frequencies:
        ref = ssvep_references(float(f), fs, len(eeg), harmonics)
        scores[float(f)] = canonical_corr(eeg, ref)
    best = max(scores, key=scores.get)
    return best, scores


if __name__ == "__main__":
    fs = 250.0
    t = np.arange(int(2 * fs)) / fs
    rng = np.random.default_rng(7)
    eeg = np.column_stack([
        10 * np.sin(2 * np.pi * 12 * t) + 2 * rng.standard_normal(len(t)),
        8 * np.sin(2 * np.pi * 12 * t + 0.4) + 2 * rng.standard_normal(len(t)),
    ])
    label, scores = predict_ssvep(eeg, fs, [8, 10, 12, 15])
    print("Predicted frequency:", label)
    print(scores)
