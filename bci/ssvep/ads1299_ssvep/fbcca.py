"""Filter-bank CCA for training-free SSVEP classification."""

from __future__ import annotations

from collections.abc import Sequence

import numpy as np
from scipy import signal

from .cca import canonical_correlation
from .core import SSVEPDecision, as_eeg, decision_from_scores, normalize_frequencies, validate_fs
from .references import harmonic_references


def default_subbands(fs: float, *, high_hz: float = 90.0) -> tuple[tuple[float, float], ...]:
    """Return practical harmonic-preserving SSVEP subbands for the sampling rate."""
    fs = validate_fs(fs)
    upper = min(float(high_hz), fs / 2 - 1.0)
    lows = (6.0, 14.0, 22.0, 30.0, 38.0)
    bands = tuple((low, upper) for low in lows if low < upper - 1.0)
    if not bands:
        raise ValueError("sampling rate is too low for the default SSVEP filter bank")
    return bands


def filter_bank_weights(n_bands: int, *, a: float = 1.25, b: float = 0.25) -> np.ndarray:
    """Return monotonically decreasing FBCCA subband weights."""
    n_bands = int(n_bands)
    if n_bands < 1:
        raise ValueError("n_bands must be positive")
    index = np.arange(1, n_bands + 1, dtype=float)
    weights = index ** (-float(a)) + float(b)
    if not np.all(np.isfinite(weights)) or np.any(weights <= 0):
        raise ValueError("invalid filter-bank weights")
    return weights


def _bandpass(eeg: np.ndarray, fs: float, low: float, high: float, order: int) -> np.ndarray:
    nyquist = fs / 2
    high = min(float(high), nyquist - max(0.5, fs * 1e-4))
    low = float(low)
    if not 0 < low < high < nyquist:
        raise ValueError("invalid filter-bank edge")
    sos = signal.butter(int(order), [low, high], btype="bandpass", fs=fs, output="sos")
    try:
        return signal.sosfiltfilt(sos, eeg, axis=0)
    except ValueError as exc:
        raise ValueError("EEG window is too short for the selected FBCCA filter bank") from exc


def fbcca_scores(
    eeg: np.ndarray,
    fs: float,
    frequencies: Sequence[float],
    *,
    harmonics: int = 3,
    subbands: Sequence[tuple[float, float]] | None = None,
    filter_order: int = 4,
    weight_a: float = 1.25,
    weight_b: float = 0.25,
    ridge: float = 1e-8,
) -> dict[float, float]:
    """Compute weighted squared CCA scores over an SSVEP filter bank."""
    fs = validate_fs(fs)
    x = as_eeg(eeg, min_samples=32)
    freqs = normalize_frequencies(frequencies, fs)
    bands = tuple(default_subbands(fs) if subbands is None else subbands)
    if not bands:
        raise ValueError("subbands is empty")
    weights = filter_bank_weights(len(bands), a=weight_a, b=weight_b)
    references = {
        f: harmonic_references(f, fs, len(x), harmonics=harmonics) for f in freqs
    }
    total = {f: 0.0 for f in freqs}
    for weight, band in zip(weights, bands, strict=True):
        if len(band) != 2:
            raise ValueError("every subband must be (low_hz, high_hz)")
        xb = _bandpass(x, fs, band[0], band[1], int(filter_order))
        for f in freqs:
            rho = canonical_correlation(xb, references[f], ridge=ridge)
            total[f] += float(weight * rho * rho)
    return total


class FBCCAClassifier:
    """Filter-bank CCA classifier with optional score/margin rejection."""

    def __init__(
        self,
        frequencies: Sequence[float],
        fs: float,
        *,
        harmonics: int = 3,
        subbands: Sequence[tuple[float, float]] | None = None,
        filter_order: int = 4,
        weight_a: float = 1.25,
        weight_b: float = 0.25,
        ridge: float = 1e-8,
        min_score: float | None = None,
        min_margin: float | None = None,
    ) -> None:
        self.fs = validate_fs(fs)
        self.frequencies = normalize_frequencies(frequencies, self.fs)
        self.harmonics = int(harmonics)
        self.subbands = None if subbands is None else tuple(tuple(map(float, b)) for b in subbands)
        self.filter_order = int(filter_order)
        self.weight_a = float(weight_a)
        self.weight_b = float(weight_b)
        self.ridge = float(ridge)
        self.min_score = min_score
        self.min_margin = min_margin

    def score_window(self, eeg: np.ndarray) -> dict[float, float]:
        return fbcca_scores(
            eeg,
            self.fs,
            self.frequencies,
            harmonics=self.harmonics,
            subbands=self.subbands,
            filter_order=self.filter_order,
            weight_a=self.weight_a,
            weight_b=self.weight_b,
            ridge=self.ridge,
        )

    def predict(self, eeg: np.ndarray) -> SSVEPDecision:
        return decision_from_scores(
            self.score_window(eeg),
            min_score=self.min_score,
            min_margin=self.min_margin,
        )
