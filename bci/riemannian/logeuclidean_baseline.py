"""Log-Euclidean covariance baseline for EEG/BCI classification.

Educational alternative to mature libraries such as pyRiemann. Epoch shape:
trials x channels x samples.
"""

from __future__ import annotations
import numpy as np


def covariance_matrices(epochs: np.ndarray, ridge: float = 1e-6) -> np.ndarray:
    ep = np.asarray(epochs, dtype=float)
    if ep.ndim != 3:
        raise ValueError("epochs must be trials x channels x samples")
    out = []
    for e in ep:
        e = e - np.mean(e, axis=1, keepdims=True)
        c = e @ e.T / max(e.shape[1]-1, 1)
        scale = np.trace(c) / c.shape[0]
        c += (ridge * max(scale, np.finfo(float).eps)) * np.eye(c.shape[0])
        out.append(c)
    return np.asarray(out)


def sym_logm(c: np.ndarray) -> np.ndarray:
    vals, vecs = np.linalg.eigh((c + c.T) / 2)
    vals = np.maximum(vals, np.finfo(float).eps)
    return (vecs * np.log(vals)) @ vecs.T


def logeuclidean_features(covs: np.ndarray) -> np.ndarray:
    covs = np.asarray(covs, dtype=float)
    n = covs.shape[1]
    iu = np.triu_indices(n)
    feats = []
    for c in covs:
        l = sym_logm(c)
        v = l[iu].copy()
        off = iu[0] != iu[1]
        v[off] *= np.sqrt(2.0)
        feats.append(v)
    return np.asarray(feats)
