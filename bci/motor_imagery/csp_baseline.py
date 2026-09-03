"""Two-class Common Spatial Pattern (CSP) baseline.

Educational/reference implementation. Epoch input shape:
trials x channels x samples.
"""

from __future__ import annotations
import numpy as np
from scipy.linalg import eigh


def _cov_trace_norm(epoch: np.ndarray) -> np.ndarray:
    c = epoch @ epoch.T
    tr = np.trace(c)
    if tr <= 0:
        raise ValueError("Degenerate epoch covariance")
    return c / tr


def fit_csp(class_a: np.ndarray, class_b: np.ndarray, n_pairs: int = 2):
    a = np.asarray(class_a, dtype=float)
    b = np.asarray(class_b, dtype=float)
    if a.ndim != 3 or b.ndim != 3 or a.shape[1] != b.shape[1]:
        raise ValueError("Expected trials x channels x samples for both classes")

    ca = np.mean([_cov_trace_norm(e) for e in a], axis=0)
    cb = np.mean([_cov_trace_norm(e) for e in b], axis=0)
    composite = ca + cb
    vals, vecs = eigh(ca, composite)
    order = np.argsort(vals)[::-1]
    vecs = vecs[:, order]
    n_pairs = min(n_pairs, a.shape[1] // 2)
    w = np.concatenate([vecs[:, :n_pairs], vecs[:, -n_pairs:]], axis=1).T
    return w


def transform_csp(epochs: np.ndarray, filters: np.ndarray) -> np.ndarray:
    epochs = np.asarray(epochs, dtype=float)
    feats = []
    for e in epochs:
        z = filters @ e
        var = np.var(z, axis=1)
        var = var / np.maximum(np.sum(var), 1e-15)
        feats.append(np.log(np.maximum(var, 1e-15)))
    return np.asarray(feats)


if __name__ == "__main__":
    rng = np.random.default_rng(1)
    a = rng.standard_normal((20, 8, 500))
    b = rng.standard_normal((20, 8, 500))
    a[:, 0, :] *= 2.0
    b[:, 7, :] *= 2.0
    w = fit_csp(a, b, n_pairs=2)
    print("CSP filters:", w.shape)
    print("Features:", transform_csp(a[:3], w).shape)
