"""Reusable EEG feature extraction helpers."""

from __future__ import annotations

import numpy as np
from scipy import stats

from .core import as_continuous
from .spectral import alpha_peak_frequency, bandpower, spectral_entropy


def time_domain_features(data: np.ndarray) -> dict[str, np.ndarray]:
    """Compute common channel-wise time-domain descriptors."""
    x, _ = as_continuous(data)
    finite = np.isfinite(x)
    if not np.all(finite):
        x = np.where(finite, x, np.nan)
    centered = x - np.nanmean(x, axis=0, keepdims=True)
    return {
        "mean": np.nanmean(x, axis=0),
        "std": np.nanstd(x, axis=0, ddof=1),
        "variance": np.nanvar(x, axis=0, ddof=1),
        "rms": np.sqrt(np.nanmean(x**2, axis=0)),
        "peak_to_peak": np.nanmax(x, axis=0) - np.nanmin(x, axis=0),
        "mean_absolute": np.nanmean(np.abs(x), axis=0),
        "mean_absolute_deviation": np.nanmean(np.abs(centered), axis=0),
        "skewness": stats.skew(x, axis=0, nan_policy="omit", bias=False),
        "kurtosis_excess": stats.kurtosis(x, axis=0, nan_policy="omit", fisher=True, bias=False),
    }


def hjorth_parameters(data: np.ndarray) -> dict[str, np.ndarray]:
    """Compute Hjorth activity, mobility and complexity per channel."""
    x, _ = as_continuous(data)
    if np.isnan(x).any():
        raise ValueError("hjorth_parameters does not accept NaN values")
    dx = np.diff(x, axis=0)
    ddx = np.diff(dx, axis=0)
    var0 = np.var(x, axis=0)
    var1 = np.var(dx, axis=0)
    var2 = np.var(ddx, axis=0)
    mobility = np.sqrt(np.divide(var1, var0, out=np.zeros_like(var0), where=var0 > 0))
    mobility_dx = np.sqrt(np.divide(var2, var1, out=np.zeros_like(var1), where=var1 > 0))
    complexity = np.divide(
        mobility_dx,
        mobility,
        out=np.zeros_like(mobility),
        where=mobility > 0,
    )
    return {"activity": var0, "mobility": mobility, "complexity": complexity}


def line_length(data: np.ndarray) -> np.ndarray:
    """Return average absolute first difference for each channel."""
    x, _ = as_continuous(data)
    return np.nanmean(np.abs(np.diff(x, axis=0)), axis=0)


def zero_crossing_rate(data: np.ndarray, threshold: float = 0.0) -> np.ndarray:
    """Fraction of adjacent samples that cross a threshold per channel."""
    x, _ = as_continuous(data)
    centered = x - float(threshold)
    valid = np.isfinite(centered[:-1]) & np.isfinite(centered[1:])
    crossings = ((centered[:-1] < 0) & (centered[1:] >= 0)) | (
        (centered[:-1] >= 0) & (centered[1:] < 0)
    )
    numerator = np.sum(crossings & valid, axis=0)
    denominator = np.sum(valid, axis=0)
    return np.divide(
        numerator,
        denominator,
        out=np.full(x.shape[1], np.nan, dtype=float),
        where=denominator > 0,
    )


def slope_sign_changes(data: np.ndarray, threshold: float = 0.0) -> np.ndarray:
    """Count normalized slope-sign changes, an educational complexity feature."""
    x, _ = as_continuous(data)
    dx = np.diff(x, axis=0)
    if dx.shape[0] < 2:
        return np.zeros(x.shape[1], dtype=float)
    left = dx[:-1]
    right = dx[1:]
    valid = np.isfinite(left) & np.isfinite(right)
    change = ((left > threshold) & (right < -threshold)) | (
        (left < -threshold) & (right > threshold)
    )
    denominator = np.sum(valid, axis=0)
    return np.divide(
        np.sum(change & valid, axis=0),
        denominator,
        out=np.full(x.shape[1], np.nan, dtype=float),
        where=denominator > 0,
    )


def extract_feature_table(
    data: np.ndarray,
    fs: float,
    *,
    include_bandpower: bool = True,
    include_alpha_peak: bool = True,
    include_entropy: bool = True,
) -> dict[str, np.ndarray]:
    """Build a flat feature dictionary suitable for ML/tabular export.

    Every value is shaped ``channels``. The function deliberately returns
    arrays rather than a pandas object so the core package has minimal
    dependencies.
    """
    features = time_domain_features(data)
    hjorth = hjorth_parameters(data)
    features.update({f"hjorth_{name}": value for name, value in hjorth.items()})
    features["line_length"] = line_length(data)
    features["zero_crossing_rate"] = zero_crossing_rate(data)
    features["slope_sign_changes"] = slope_sign_changes(data)

    if include_bandpower:
        bp = bandpower(data, fs)
        for name, value in bp.absolute.items():
            features[f"bandpower_{name}_absolute"] = value
        for name, value in bp.relative.items():
            features[f"bandpower_{name}_relative"] = value
    if include_alpha_peak:
        features["alpha_peak_hz"] = alpha_peak_frequency(data, fs)
    if include_entropy:
        features["spectral_entropy"] = spectral_entropy(data, fs)
    return features
