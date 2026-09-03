"""Backward-compatible FBCCA entry point for ADS1299-Core-SDK.

The reusable implementation is maintained in ``ads1299_ssvep.fbcca``. This
wrapper preserves the older ``predict_fbcca`` function used by examples.
"""

from __future__ import annotations

from ads1299_ssvep import FBCCAClassifier


def predict_fbcca(
    eeg,
    fs: float,
    frequencies,
    harmonics: int = 3,
    subband_lows=(6.0, 14.0, 22.0, 30.0, 38.0),
    high_hz: float = 90.0,
    weight_a: float = 1.25,
    weight_b: float = 0.25,
):
    upper = min(float(high_hz), float(fs) / 2.0 - 1.0)
    subbands = tuple((float(low), upper) for low in subband_lows if float(low) < upper - 1.0)
    classifier = FBCCAClassifier(
        frequencies,
        fs,
        harmonics=harmonics,
        subbands=subbands,
        weight_a=weight_a,
        weight_b=weight_b,
    )
    decision = classifier.predict(eeg)
    return decision.label_hz, decision.scores


if __name__ == "__main__":
    import numpy as np

    fs = 250.0
    t = np.arange(int(3 * fs)) / fs
    rng = np.random.default_rng(9)
    eeg = np.column_stack(
        [
            8 * np.sin(2 * np.pi * 12 * t)
            + 3 * np.sin(2 * np.pi * 24 * t)
            + rng.normal(0, 2, len(t)),
            7 * np.sin(2 * np.pi * 12 * t + 0.5)
            + 2 * np.sin(2 * np.pi * 24 * t)
            + rng.normal(0, 2, len(t)),
        ]
    )
    label, scores = predict_fbcca(eeg, fs, [8, 10, 12, 15])
    print("Predicted frequency:", label)
    print(scores)
