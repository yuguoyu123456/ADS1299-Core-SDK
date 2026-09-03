"""Filter-bank CCA (FBCCA) educational baseline for SSVEP.

Input EEG: samples x channels, in any consistent amplitude unit.
Requires NumPy + SciPy. This is a transparent starter implementation rather
than a reproduction of a specific benchmark paper.
"""

from __future__ import annotations
import numpy as np
from scipy import signal
from cca_baseline import canonical_corr, ssvep_references


def _subband(eeg: np.ndarray, fs: float, low: float, high: float) -> np.ndarray:
    nyq = fs / 2.0
    high = min(high, nyq - 0.5)
    if not 0 < low < high:
        raise ValueError("Invalid filter-band edges")
    sos = signal.butter(4, [low, high], btype="bandpass", fs=fs, output="sos")
    return signal.sosfiltfilt(sos, eeg, axis=0)


def predict_fbcca(
    eeg: np.ndarray,
    fs: float,
    frequencies,
    harmonics: int = 3,
    subband_lows=(6.0, 14.0, 22.0, 30.0, 38.0),
    high_hz: float = 90.0,
    weight_a: float = 1.25,
    weight_b: float = 0.25,
):
    x = np.asarray(eeg, dtype=float)
    if x.ndim == 1:
        x = x[:, None]
    if x.ndim != 2:
        raise ValueError("eeg must be samples x channels")

    freqs = [float(f) for f in frequencies]
    references = {
        f: ssvep_references(f, fs, len(x), harmonics=harmonics) for f in freqs
    }
    total = {f: 0.0 for f in freqs}

    for index, low in enumerate(subband_lows, start=1):
        if low >= fs / 2 - 0.5:
            continue
        xb = _subband(x, fs, low, high_hz)
        weight = index ** (-weight_a) + weight_b
        for f in freqs:
            rho = canonical_corr(xb, references[f])
            total[f] += weight * (rho ** 2)

    if not any(np.isfinite(list(total.values()))):
        raise ValueError("No usable filter bank for this sampling rate")
    best = max(total, key=total.get)
    return best, total


if __name__ == "__main__":
    fs = 250.0
    t = np.arange(int(3 * fs)) / fs
    rng = np.random.default_rng(9)
    eeg = np.column_stack([
        8*np.sin(2*np.pi*12*t) + 3*np.sin(2*np.pi*24*t) + rng.normal(0, 2, len(t)),
        7*np.sin(2*np.pi*12*t + .5) + 2*np.sin(2*np.pi*24*t) + rng.normal(0, 2, len(t)),
    ])
    label, scores = predict_fbcca(eeg, fs, [8, 10, 12, 15])
    print("Predicted frequency:", label)
    print(scores)
