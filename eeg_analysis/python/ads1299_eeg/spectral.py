"""Frequency-domain EEG analysis helpers."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Mapping

import numpy as np
from scipy import signal

from .core import DEFAULT_EEG_BANDS, as_continuous, validate_sampling_rate


@dataclass(frozen=True)
class BandPowerResult:
    """Absolute and relative power for each named frequency band."""

    absolute: dict[str, np.ndarray]
    relative: dict[str, np.ndarray]
    total: np.ndarray
    frequencies: np.ndarray
    psd: np.ndarray


def welch_psd(
    data: np.ndarray,
    fs: float,
    *,
    nperseg: int | None = None,
    noverlap: int | None = None,
    detrend: str | bool = "constant",
    window: str = "hann",
) -> tuple[np.ndarray, np.ndarray]:
    """Estimate PSD with Welch's method.

    Returns ``frequencies`` and ``psd`` shaped ``frequency x channels``.
    """
    fs = validate_sampling_rate(fs)
    x, _ = as_continuous(data)
    if np.isnan(x).any():
        raise ValueError("welch_psd does not accept NaN values")
    if nperseg is None:
        nperseg = min(x.shape[0], max(64, int(round(2.0 * fs))))
    nperseg = int(nperseg)
    if nperseg < 8:
        raise ValueError("nperseg must be at least 8")
    nperseg = min(nperseg, x.shape[0])
    if noverlap is None:
        noverlap = nperseg // 2
    if not 0 <= noverlap < nperseg:
        raise ValueError("noverlap must satisfy 0 <= noverlap < nperseg")
    frequencies, psd = signal.welch(
        x,
        fs=fs,
        axis=0,
        window=window,
        nperseg=nperseg,
        noverlap=noverlap,
        detrend=detrend,
        scaling="density",
    )
    return frequencies, psd


def integrate_psd(
    frequencies: np.ndarray,
    psd: np.ndarray,
    low: float,
    high: float,
    *,
    include_high: bool = False,
) -> np.ndarray:
    """Integrate PSD over a frequency interval for every channel."""
    frequencies = np.asarray(frequencies, dtype=float)
    psd = np.asarray(psd, dtype=float)
    if frequencies.ndim != 1 or psd.ndim != 2 or psd.shape[0] != frequencies.size:
        raise ValueError("expected 1-D frequencies and frequency x channels PSD")
    if high <= low:
        raise ValueError("high must be greater than low")
    mask = (frequencies >= low) & (frequencies <= high if include_high else frequencies < high)
    if np.count_nonzero(mask) < 2:
        return np.full(psd.shape[1], np.nan, dtype=float)
    return np.trapezoid(psd[mask], frequencies[mask], axis=0)


def bandpower(
    data: np.ndarray,
    fs: float,
    *,
    bands: Mapping[str, tuple[float, float]] | None = None,
    total_range: tuple[float, float] = (0.5, 45.0),
    nperseg: int | None = None,
) -> BandPowerResult:
    """Compute absolute and relative EEG band power using Welch PSD."""
    selected_bands = dict(DEFAULT_EEG_BANDS if bands is None else bands)
    frequencies, psd = welch_psd(data, fs, nperseg=nperseg)
    total = integrate_psd(frequencies, psd, total_range[0], total_range[1], include_high=True)

    absolute: dict[str, np.ndarray] = {}
    relative: dict[str, np.ndarray] = {}
    for name, limits in selected_bands.items():
        low, high = limits
        power = integrate_psd(frequencies, psd, low, high)
        absolute[name] = power
        relative[name] = np.divide(
            power,
            total,
            out=np.full_like(power, np.nan, dtype=float),
            where=np.isfinite(total) & (total > 0),
        )
    return BandPowerResult(absolute, relative, total, frequencies, psd)


def dominant_frequency(
    data: np.ndarray,
    fs: float,
    *,
    frequency_range: tuple[float, float] = (1.0, 45.0),
    nperseg: int | None = None,
) -> np.ndarray:
    """Return the maximum-PSD frequency for each channel in a selected range."""
    frequencies, psd = welch_psd(data, fs, nperseg=nperseg)
    low, high = frequency_range
    mask = (frequencies >= low) & (frequencies <= high)
    if not np.any(mask):
        raise ValueError("frequency_range does not contain any PSD bins")
    local_frequencies = frequencies[mask]
    local_psd = psd[mask]
    return local_frequencies[np.argmax(local_psd, axis=0)]


def alpha_peak_frequency(
    data: np.ndarray,
    fs: float,
    *,
    alpha_range: tuple[float, float] = (8.0, 13.0),
    nperseg: int | None = None,
) -> np.ndarray:
    """Return the strongest alpha-band frequency for each channel."""
    return dominant_frequency(data, fs, frequency_range=alpha_range, nperseg=nperseg)


def spectral_entropy(
    data: np.ndarray,
    fs: float,
    *,
    frequency_range: tuple[float, float] = (0.5, 45.0),
    nperseg: int | None = None,
    normalize: bool = True,
) -> np.ndarray:
    """Compute channel-wise Shannon entropy of normalized spectral power."""
    frequencies, psd = welch_psd(data, fs, nperseg=nperseg)
    low, high = frequency_range
    mask = (frequencies >= low) & (frequencies <= high)
    if np.count_nonzero(mask) < 2:
        raise ValueError("frequency_range contains too few PSD bins")
    p = np.maximum(psd[mask], 0.0)
    denominator = np.sum(p, axis=0, keepdims=True)
    p = np.divide(p, denominator, out=np.zeros_like(p), where=denominator > 0)
    entropy = -np.sum(np.where(p > 0, p * np.log2(p), 0.0), axis=0)
    if normalize:
        maximum = np.log2(p.shape[0])
        if maximum > 0:
            entropy = entropy / maximum
    return entropy


def line_noise_ratio(
    data: np.ndarray,
    fs: float,
    *,
    mains_frequency: float = 50.0,
    line_half_width: float = 1.0,
    reference_band: tuple[float, float] = (1.0, 45.0),
    nperseg: int | None = None,
) -> np.ndarray:
    """Return line-noise power divided by broadband reference power.

    For a 60 Hz environment, set ``mains_frequency=60`` and ensure the sample
    rate/reference band allow that frequency to be represented.
    """
    frequencies, psd = welch_psd(data, fs, nperseg=nperseg)
    if mains_frequency >= fs / 2:
        return np.full(psd.shape[1], np.nan, dtype=float)
    line = integrate_psd(
        frequencies,
        psd,
        max(0.0, mains_frequency - line_half_width),
        mains_frequency + line_half_width,
        include_high=True,
    )
    broadband = integrate_psd(
        frequencies,
        psd,
        reference_band[0],
        min(reference_band[1], fs / 2 - np.finfo(float).eps),
        include_high=True,
    )
    return np.divide(
        line,
        broadband,
        out=np.full_like(line, np.nan, dtype=float),
        where=np.isfinite(broadband) & (broadband > 0),
    )


def relative_spectrum(
    data: np.ndarray,
    fs: float,
    *,
    nperseg: int | None = None,
) -> tuple[np.ndarray, np.ndarray]:
    """Return Welch PSD normalized to unit total power per channel."""
    frequencies, psd = welch_psd(data, fs, nperseg=nperseg)
    total = np.trapezoid(psd, frequencies, axis=0)
    normalized = np.divide(
        psd,
        total[None, :],
        out=np.zeros_like(psd),
        where=total[None, :] > 0,
    )
    return frequencies, normalized
