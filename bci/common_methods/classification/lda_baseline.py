"""Small regularized linear discriminant analysis baseline.

Useful after CSP, P300 or other handcrafted feature extraction. Kept NumPy-only
so the SDK has a transparent classifier even without scikit-learn.
"""

from __future__ import annotations
import numpy as np


class RegularizedLDA:
    def __init__(self, shrinkage: float = 0.1):
        if not 0 <= shrinkage <= 1:
            raise ValueError("shrinkage must be in [0, 1]")
        self.shrinkage = float(shrinkage)

    def fit(self, x, y):
        x = np.asarray(x, dtype=float)
        y = np.asarray(y)
        if x.ndim != 2 or len(x) != len(y):
            raise ValueError("x must be observations x features and align with y")
        classes = np.unique(y)
        if len(classes) != 2:
            raise ValueError("This transparent baseline currently supports two classes")

        self.classes_ = classes
        means = []
        cov_sum = np.zeros((x.shape[1], x.shape[1]), dtype=float)
        denom = 0
        priors = []
        for c in classes:
            xc = x[y == c]
            if len(xc) < 2:
                raise ValueError("Need at least two observations per class")
            mu = np.mean(xc, axis=0)
            means.append(mu)
            centered = xc - mu
            cov_sum += centered.T @ centered
            denom += len(xc) - 1
            priors.append(len(xc) / len(x))

        cov = cov_sum / max(denom, 1)
        target = np.trace(cov) / cov.shape[0] * np.eye(cov.shape[0])
        cov = (1-self.shrinkage)*cov + self.shrinkage*target
        self.inv_cov_ = np.linalg.pinv(cov)
        self.means_ = np.asarray(means)
        self.priors_ = np.asarray(priors)
        return self

    def decision_function(self, x):
        x = np.asarray(x, dtype=float)
        scores = []
        for mu, prior in zip(self.means_, self.priors_):
            scores.append(x @ self.inv_cov_ @ mu - 0.5*mu @ self.inv_cov_ @ mu + np.log(prior))
        return np.column_stack(scores)

    def predict(self, x):
        scores = self.decision_function(x)
        return self.classes_[np.argmax(scores, axis=1)]
