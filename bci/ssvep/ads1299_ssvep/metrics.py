"""Evaluation metrics for SSVEP experiments."""

from __future__ import annotations

from collections.abc import Sequence

import numpy as np


def accuracy_score(y_true: Sequence[float], y_pred: Sequence[float | None]) -> float:
    truth = np.asarray(y_true, dtype=float)
    pred = np.asarray([np.nan if v is None else float(v) for v in y_pred], dtype=float)
    if truth.ndim != 1 or pred.ndim != 1 or len(truth) != len(pred) or len(truth) == 0:
        raise ValueError("y_true and y_pred must be non-empty equally sized vectors")
    return float(np.mean(np.isfinite(pred) & (pred == truth)))


def confusion_matrix(
    y_true: Sequence[float],
    y_pred: Sequence[float | None],
    labels: Sequence[float],
) -> np.ndarray:
    classes = tuple(float(v) for v in labels)
    if len(classes) == 0 or len(set(classes)) != len(classes):
        raise ValueError("labels must be non-empty and unique")
    index = {label: i for i, label in enumerate(classes)}
    matrix = np.zeros((len(classes), len(classes)), dtype=int)
    if len(y_true) != len(y_pred):
        raise ValueError("y_true and y_pred lengths differ")
    for truth, pred in zip(y_true, y_pred, strict=True):
        t = float(truth)
        if t not in index:
            raise ValueError(f"unknown true label {t}")
        if pred is None:
            continue
        p = float(pred)
        if p not in index:
            raise ValueError(f"unknown predicted label {p}")
        matrix[index[t], index[p]] += 1
    return matrix


def itr_bits_per_minute(n_targets: int, accuracy: float, trial_seconds: float) -> float:
    """Wolpaw-style information transfer rate in bits/minute.

    ``trial_seconds`` should include the complete decision cycle relevant to
    the experiment (analysis window plus any mandatory transition/selection
    time), not only the DSP compute time.
    """
    n = int(n_targets)
    p = float(accuracy)
    t = float(trial_seconds)
    if n < 2:
        raise ValueError("n_targets must be at least two")
    if not 0 <= p <= 1 or not np.isfinite(p):
        raise ValueError("accuracy must lie in [0, 1]")
    if not np.isfinite(t) or t <= 0:
        raise ValueError("trial_seconds must be positive")

    if p <= 0:
        bits = np.log2(n) + np.log2(1.0 / (n - 1))
    elif p >= 1:
        bits = np.log2(n)
    else:
        bits = (
            np.log2(n)
            + p * np.log2(p)
            + (1 - p) * np.log2((1 - p) / (n - 1))
        )
    # The symmetric-channel formula can become negative below chance; negative
    # information rate is not useful as a BCI throughput report.
    return float(max(0.0, bits) * 60.0 / t)
