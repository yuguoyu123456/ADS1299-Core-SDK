import sys
from pathlib import Path
import numpy as np

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "bci" / "ssvep"))
sys.path.insert(0, str(ROOT / "bci" / "motor_imagery"))
sys.path.insert(0, str(ROOT / "bci" / "common_methods" / "classification"))
sys.path.insert(0, str(ROOT / "bci" / "common_methods" / "riemannian"))

from fbcca_baseline import predict_fbcca
from erd_ers import erd_ers
from lda_baseline import RegularizedLDA
from logeuclidean_baseline import covariance_matrices, logeuclidean_features

# FBCCA should identify a clean 12-Hz synthetic SSVEP.
fs = 250.0
t = np.arange(int(3 * fs)) / fs
rng = np.random.default_rng(12)
eeg = np.column_stack([
    10*np.sin(2*np.pi*12*t) + 3*np.sin(2*np.pi*24*t) + rng.normal(0, 1, len(t)),
    8*np.sin(2*np.pi*12*t + .4) + 2*np.sin(2*np.pi*24*t) + rng.normal(0, 1, len(t)),
])
label, _ = predict_fbcca(eeg, fs, [8, 10, 12, 15])
assert label == 12.0

# ERD/ERS output shape and finiteness.
times = np.arange(-2.0, 3.0, 1/fs)
epochs = rng.normal(0, 1, (6, len(times), 4))
percent, mean_percent = erd_ers(epochs, fs, times, band=(8, 13), baseline=(-1.5, -0.5))
assert percent.shape == epochs.shape
assert mean_percent.shape == epochs.shape[1:]
assert np.all(np.isfinite(mean_percent))

# Log-Euclidean covariance features + transparent LDA.
a = rng.normal(0, 1, (20, 4, 250))
b = rng.normal(0, 1, (20, 4, 250))
a[:, 0, :] *= 2.0
b[:, 3, :] *= 2.0
all_epochs = np.concatenate([a, b], axis=0)
y = np.array([0]*len(a) + [1]*len(b))
features = logeuclidean_features(covariance_matrices(all_epochs))
clf = RegularizedLDA(0.2).fit(features, y)
pred = clf.predict(features)
assert np.mean(pred == y) > 0.8

print("extended BCI smoke tests passed")
