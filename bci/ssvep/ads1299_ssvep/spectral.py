"""Frequency-domain SSVEP scoring helpers."""

from __future__ import annotations

from collections.abc import Sequence

import numpy as np
from scipy import signal

from .core import as_eeg, normalize_frequencies, validate_fs


def _one_sided_power(eeg: np.ndarray, fs: float) -> tuple[np.ndarray, np.ndarray]:
    x = as_eeg(eeg)
    fs = validate_fs(fs)
    x = signal.detrend(x, axis=0, type="linear")
    window = signal.windows.hann(len(x), sym=False)[:, None]
    spectrum = np.fft.rfft(x * window, axis=0)
    power = np.mean(np.abs(spectrum) ** 2, axis=1)
    freq = np.fft.rfftfreq(len(x), d=1.0 / fs)
    return freq, power


def harmonic_power_score(
    eeg: np.ndarray,
    fs: float,
    frequency_hz: float,
    *,
    harmonics: int = 3,
    bin_half_width_hz: float = 0.5,
) -> float:
    """Sum FFT power near a target and its usable harmonics."""
    freq, power = _one_sided_power(eeg, fs)
    target = float(frequency_hz)
    if target <= 0 or target >= fs / 2:
        raise ValueError("frequency_hz must lie inside (0, Nyquist)")
    total = 0.0
    used = 0
    for h in range(1, int(harmonics) + 1):
        hf = h * target
        if hf >= fs / 2:
            break
        mask = np.abs(freq - hf) <= float(bin_half_width_hz)
        if np.any(mask):
            total += float(np.sum(power[mask]))
            used += 1
    if used == 0:
        raise ValueError("no usable target harmonic")
    return total


def frequency_snr_db(
    eeg: np.ndarray,
    fs: float,
    frequency_hz: float,
    *,
    harmonics: int = 3,
    signal_half_width_hz: float = 0.5,
    noise_inner_hz: float = 1.0,
    noise_outer_hz: float = 3.0,
) -> float:
    """Estimate harmonic SSVEP SNR in dB using local neighboring FFT bins.

    Target bins around each harmonic are compared with flanking bins that are
    close enough to reflect the local spectral floor but exclude the target.
    """
    freq, power = _one_sided_power(eeg, fs)
    target = float(frequency_hz)
    if not (0 < target < fs / 2):
        raise ValueError("frequency_hz must lie inside (0, Nyquist)")
    if not (0 < signal_half_width_hz < noise_inner_hz < noise_outer_hz):
        raise ValueError("require 0 < signal width < noise inner < noise outer")

    signal_power = 0.0
    noise_power = 0.0
    signal_bins = 0
    noise_bins = 0
    for h in range(1, int(harmonics) + 1):
        hf = h * target
        if hf >= fs / 2:
            break
        distance = np.abs(freq - hf)
        smask = distance <= signal_half_width_hz
        nmask = (distance >= noise_inner_hz) & (distance <= noise_outer_hz)
        if np.any(smask) and np.any(nmask):
            signal_power += float(np.sum(power[smask]))
            noise_power += float(np.sum(power[nmask]))
            signal_bins += int(np.count_nonzero(smask))
            noise_bins += int(np.count_nonzero(nmask))
    if signal_bins == 0 or noise_bins == 0:
        raise ValueError("window is too short to estimate local SNR")
    signal_mean = signal_power / signal_bins
    noise_mean = noise_power / noise_bins
    return float(10.0 * np.log10(max(signal_mean, np.finfo(float).tiny) / max(noise_mean, np.finfo(float).tiny)))


def spectral_scores(
    eeg: np.ndarray,
    fs: float,
    frequencies: Sequence[float],
    *,
    harmonics: int = 3,
    metric: str = "snr_db",
) -> dict[float, float]:
    """Score all candidate frequencies using SNR or harmonic power."""
    fs = validate_fs(fs)
    freqs = normalize_frequencies(frequencies, fs)
    if metric == "snr_db":
        return {f: frequency_snr_db(eeg, fs, f, harmonics=harmonics) for f in freqs}
    if metric == "power":
        return {f: harmonic_power_score(eeg, fs, f, harmonics=harmonics) for f in freqs}
    raise ValueError("metric must be 'snr_db' or 'power'")
