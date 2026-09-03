"""Backward-compatible CCA entry point for ADS1299-Core-SDK.

The maintained implementation now lives in the installable ``ads1299_ssvep``
package. This file keeps the historical function names so existing tutorials
and user scripts do not silently break.
"""

from __future__ import annotations

import numpy as np

from ads1299_ssvep import CCAClassifier, canonical_correlation, harmonic_references


def canonical_corr(x: np.ndarray, y: np.ndarray, ridge: float = 1e-8) -> float:
    return canonical_correlation(x, y, ridge=ridge)


def ssvep_references(
    freq_hz: float,
    fs: float,
    n_samples: int,
    harmonics: int = 3,
) -> np.ndarray:
    return harmonic_references(freq_hz, fs, n_samples, harmonics=harmonics)


def predict_ssvep(eeg: np.ndarray, fs: float, frequencies, harmonics: int = 3):
    classifier = CCAClassifier(frequencies, fs, harmonics=harmonics)
    decision = classifier.predict(eeg)
    return decision.label_hz, decision.scores


if __name__ == "__main__":
    fs = 250.0
    t = np.arange(int(2 * fs)) / fs
    rng = np.random.default_rng(7)
    eeg = np.column_stack(
        [
            10 * np.sin(2 * np.pi * 12 * t) + 2 * rng.standard_normal(len(t)),
            8 * np.sin(2 * np.pi * 12 * t + 0.4) + 2 * rng.standard_normal(len(t)),
        ]
    )
    label, scores = predict_ssvep(eeg, fs, [8, 10, 12, 15])
    print("Predicted frequency:", label)
    print(scores)
