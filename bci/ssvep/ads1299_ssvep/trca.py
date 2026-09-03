"""Calibration-based Task-Related Component Analysis (TRCA) for SSVEP."""

from __future__ import annotations

from collections.abc import Sequence

import numpy as np
from scipy import linalg, signal

from .core import SSVEPDecision, as_eeg, decision_from_scores, validate_fs


def _validate_epochs(epochs: np.ndarray) -> np.ndarray:
    x = np.asarray(epochs, dtype=float)
    if x.ndim != 3:
        raise ValueError("epochs must be trials x samples x channels")
    if x.shape[0] < 2 or x.shape[1] < 8 or x.shape[2] < 1:
        raise ValueError("epochs require >=2 trials, >=8 samples and >=1 channel")
    if not np.all(np.isfinite(x)):
        raise ValueError("epochs contain NaN or Inf")
    return x


def trca_spatial_filter(epochs: np.ndarray, *, ridge: float = 1e-6) -> np.ndarray:
    """Estimate the leading TRCA spatial filter for same-class trials.

    The generalized eigenproblem maximizes reproducible inter-trial covariance
    relative to total within-trial covariance. A trace-scaled diagonal ridge
    makes the denominator well-conditioned for short calibration sets.
    """
    trials = _validate_epochs(epochs)
    centered = signal.detrend(trials, axis=1, type="constant")
    n_trials, _, n_channels = centered.shape

    q = np.zeros((n_channels, n_channels), dtype=float)
    s = np.zeros_like(q)
    for i in range(n_trials):
        xi = centered[i]
        q += xi.T @ xi
        for j in range(i + 1, n_trials):
            xj = centered[j]
            cross = xi.T @ xj
            s += cross + cross.T

    ridge = float(ridge)
    if not np.isfinite(ridge) or ridge <= 0:
        raise ValueError("ridge must be positive and finite")
    scale = np.trace(q) / max(n_channels, 1)
    q_reg = q + max(scale * ridge, np.finfo(float).eps) * np.eye(n_channels)
    values, vectors = linalg.eigh(s, q_reg, check_finite=True)
    w = np.asarray(vectors[:, np.argmax(values)], dtype=float)
    norm = np.linalg.norm(w)
    if not np.isfinite(norm) or norm <= np.finfo(float).eps:
        raise ValueError("TRCA spatial filter is degenerate")
    return w / norm


def _corr(a: np.ndarray, b: np.ndarray) -> float:
    a = np.asarray(a, dtype=float).ravel()
    b = np.asarray(b, dtype=float).ravel()
    n = min(len(a), len(b))
    if n < 4:
        raise ValueError("too few samples for correlation")
    a = a[:n] - np.mean(a[:n])
    b = b[:n] - np.mean(b[:n])
    denom = np.linalg.norm(a) * np.linalg.norm(b)
    if denom <= np.finfo(float).eps:
        return 0.0
    return float(np.clip((a @ b) / denom, -1.0, 1.0))


class TRCAClassifier:
    """Supervised SSVEP classifier using class-specific TRCA templates."""

    def __init__(
        self,
        fs: float,
        *,
        ridge: float = 1e-6,
        min_score: float | None = None,
        min_margin: float | None = None,
        absolute_correlation: bool = True,
    ) -> None:
        self.fs = validate_fs(fs)
        self.ridge = float(ridge)
        self.min_score = min_score
        self.min_margin = min_margin
        self.absolute_correlation = bool(absolute_correlation)
        self.classes_: tuple[float, ...] | None = None
        self.filters_: dict[float, np.ndarray] = {}
        self.templates_: dict[float, np.ndarray] = {}

    def fit(self, epochs: np.ndarray, labels: Sequence[float]) -> "TRCAClassifier":
        x = np.asarray(epochs, dtype=float)
        if x.ndim != 3:
            raise ValueError("epochs must be trials x samples x channels")
        y = np.asarray(labels, dtype=float)
        if y.ndim != 1 or len(y) != x.shape[0]:
            raise ValueError("labels must contain one value per trial")
        if not np.all(np.isfinite(y)):
            raise ValueError("labels contain non-finite values")
        classes = tuple(float(v) for v in np.unique(y))
        if len(classes) < 2:
            raise ValueError("TRCA requires at least two classes")

        filters: dict[float, np.ndarray] = {}
        templates: dict[float, np.ndarray] = {}
        for cls in classes:
            trials = _validate_epochs(x[y == cls])
            filters[cls] = trca_spatial_filter(trials, ridge=self.ridge)
            templates[cls] = np.mean(trials, axis=0)
        self.classes_ = classes
        self.filters_ = filters
        self.templates_ = templates
        return self

    def _require_fit(self) -> None:
        if self.classes_ is None:
            raise RuntimeError("TRCAClassifier.fit must be called before prediction")

    def score_window(self, eeg: np.ndarray) -> dict[float, float]:
        self._require_fit()
        x = as_eeg(eeg)
        scores: dict[float, float] = {}
        assert self.classes_ is not None
        for cls in self.classes_:
            w = self.filters_[cls]
            template = self.templates_[cls]
            n = min(len(x), len(template))
            if x.shape[1] != template.shape[1]:
                raise ValueError("EEG channel count differs from TRCA calibration data")
            projected_x = signal.detrend(x[:n], axis=0, type="constant") @ w
            projected_template = signal.detrend(template[:n], axis=0, type="constant") @ w
            score = _corr(projected_x, projected_template)
            if self.absolute_correlation:
                score = abs(score)
            scores[cls] = float(score)
        return scores

    def predict(self, eeg: np.ndarray) -> SSVEPDecision:
        return decision_from_scores(
            self.score_window(eeg),
            min_score=self.min_score,
            min_margin=self.min_margin,
        )
