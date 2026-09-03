"""Canonical-correlation SSVEP classification."""

from __future__ import annotations

from collections.abc import Sequence

import numpy as np

from .core import (
    SSVEPDecision,
    as_eeg,
    decision_from_scores,
    normalize_frequencies,
    standardize_columns,
    validate_fs,
)
from .references import harmonic_references


def canonical_correlation(x: np.ndarray, y: np.ndarray, *, ridge: float = 1e-8) -> float:
    """Return the largest regularized canonical correlation.

    The implementation whitens both covariance matrices with symmetric eigen
    decompositions and then takes the largest singular value of the whitened
    cross-covariance. This avoids a hard dependency on scikit-learn and keeps
    the numerical assumptions visible.
    """
    x = standardize_columns(np.asarray(x, dtype=float))
    y = standardize_columns(np.asarray(y, dtype=float))
    if x.ndim != 2 or y.ndim != 2:
        raise ValueError("x and y must both be 2-D")
    n = min(x.shape[0], y.shape[0])
    if n < 4:
        raise ValueError("at least four paired samples are required")
    x = x[:n]
    y = y[:n]
    ridge = float(ridge)
    if not np.isfinite(ridge) or ridge <= 0:
        raise ValueError("ridge must be positive and finite")

    cxx = (x.T @ x) / (n - 1) + ridge * np.eye(x.shape[1])
    cyy = (y.T @ y) / (n - 1) + ridge * np.eye(y.shape[1])
    cxy = (x.T @ y) / (n - 1)

    def invsqrt(cov: np.ndarray) -> np.ndarray:
        values, vectors = np.linalg.eigh(cov)
        floor = max(ridge, np.finfo(float).eps)
        values = np.maximum(values, floor)
        return (vectors * (1.0 / np.sqrt(values))) @ vectors.T

    whitened = invsqrt(cxx) @ cxy @ invsqrt(cyy)
    singular = np.linalg.svd(whitened, compute_uv=False)
    if singular.size == 0:
        raise ValueError("canonical correlation could not be computed")
    return float(np.clip(singular[0], 0.0, 1.0))


def cca_scores(
    eeg: np.ndarray,
    fs: float,
    frequencies: Sequence[float],
    *,
    harmonics: int = 3,
    ridge: float = 1e-8,
    detrend: bool = True,
) -> dict[float, float]:
    """Score one EEG window against every candidate frequency."""
    fs = validate_fs(fs)
    x = as_eeg(eeg)
    freqs = normalize_frequencies(frequencies, fs)
    if detrend:
        from scipy import signal

        x = signal.detrend(x, axis=0, type="linear")
    scores: dict[float, float] = {}
    for freq in freqs:
        ref = harmonic_references(freq, fs, len(x), harmonics=harmonics)
        scores[freq] = canonical_correlation(x, ref, ridge=ridge)
    return scores


class CCAClassifier:
    """Training-free SSVEP classifier based on sinusoidal CCA references."""

    def __init__(
        self,
        frequencies: Sequence[float],
        fs: float,
        *,
        harmonics: int = 3,
        ridge: float = 1e-8,
        min_score: float | None = None,
        min_margin: float | None = None,
        detrend: bool = True,
    ) -> None:
        self.fs = validate_fs(fs)
        self.frequencies = normalize_frequencies(frequencies, self.fs)
        self.harmonics = int(harmonics)
        if self.harmonics < 1:
            raise ValueError("harmonics must be >= 1")
        self.ridge = float(ridge)
        self.min_score = min_score
        self.min_margin = min_margin
        self.detrend = bool(detrend)

    def score_window(self, eeg: np.ndarray) -> dict[float, float]:
        return cca_scores(
            eeg,
            self.fs,
            self.frequencies,
            harmonics=self.harmonics,
            ridge=self.ridge,
            detrend=self.detrend,
        )

    def predict(self, eeg: np.ndarray) -> SSVEPDecision:
        return decision_from_scores(
            self.score_window(eeg),
            min_score=self.min_score,
            min_margin=self.min_margin,
        )
