"""EEG preprocessing utilities.

The functions in this module are intentionally explicit and conservative.
They are suitable for learning, engineering validation and starter research
pipelines. Filter parameters should still be chosen for the experiment rather
than copied blindly.
"""

from __future__ import annotations

from fractions import Fraction
from typing import Sequence

import numpy as np
from scipy import signal

from .core import (
    as_continuous,
    normalize_channel_indices,
    restore_channel_shape,
    validate_frequency_range,
    validate_sampling_rate,
)


def remove_dc(data: np.ndarray) -> np.ndarray:
    """Subtract each channel mean while preserving input dimensionality."""
    x, was_1d = as_continuous(data)
    y = x - np.nanmean(x, axis=0, keepdims=True)
    return restore_channel_shape(y, was_1d)


def detrend(data: np.ndarray, kind: str = "linear") -> np.ndarray:
    """Remove a constant or linear trend channel-by-channel."""
    if kind not in {"constant", "linear"}:
        raise ValueError("kind must be 'constant' or 'linear'")
    x, was_1d = as_continuous(data)
    if np.isnan(x).any():
        raise ValueError("detrend does not accept NaN values")
    y = signal.detrend(x, axis=0, type=kind)
    return restore_channel_shape(y, was_1d)


def common_average_reference(data: np.ndarray) -> np.ndarray:
    """Apply common-average reference across channels at each sample."""
    x, was_1d = as_continuous(data)
    if x.shape[1] < 2:
        raise ValueError("common-average reference requires at least two channels")
    y = x - np.nanmean(x, axis=1, keepdims=True)
    return restore_channel_shape(y, was_1d)


def rereference(data: np.ndarray, reference_channels: Sequence[int]) -> np.ndarray:
    """Reference all channels to the mean of selected reference channels."""
    x, was_1d = as_continuous(data)
    refs = normalize_channel_indices(reference_channels, x.shape[1])
    reference = np.nanmean(x[:, refs], axis=1, keepdims=True)
    y = x - reference
    return restore_channel_shape(y, was_1d)


def bipolar_reference(data: np.ndarray, pairs: Sequence[tuple[int, int]]) -> np.ndarray:
    """Create bipolar derivations ``channel_a - channel_b``.

    The output is always two-dimensional and has one column per pair.
    """
    x, _ = as_continuous(data)
    if not pairs:
        raise ValueError("at least one bipolar pair is required")
    out = np.empty((x.shape[0], len(pairs)), dtype=float)
    for column, (positive, negative) in enumerate(pairs):
        if positive < 0 or positive >= x.shape[1] or negative < 0 or negative >= x.shape[1]:
            raise IndexError("bipolar channel index out of range")
        out[:, column] = x[:, positive] - x[:, negative]
    return out


def notch_filter(
    data: np.ndarray,
    fs: float,
    freq: float = 50.0,
    q: float = 30.0,
    *,
    harmonics: int = 1,
) -> np.ndarray:
    """Apply zero-phase IIR notch filtering at mains frequency and harmonics.

    Harmonics above Nyquist are skipped automatically.
    """
    fs = validate_sampling_rate(fs)
    if freq <= 0 or q <= 0 or harmonics < 1:
        raise ValueError("freq, q and harmonics must be positive")
    x, was_1d = as_continuous(data)
    if np.isnan(x).any():
        raise ValueError("notch_filter does not accept NaN values")
    y = x.copy()
    for harmonic in range(1, harmonics + 1):
        current = float(freq) * harmonic
        if current >= fs / 2:
            break
        b, a = signal.iirnotch(current, Q=float(q), fs=fs)
        padlen = 3 * (max(len(a), len(b)) - 1)
        if y.shape[0] <= padlen:
            raise ValueError("signal is too short for zero-phase notch filtering")
        y = signal.filtfilt(b, a, y, axis=0)
    return restore_channel_shape(y, was_1d)


def bandpass_filter(
    data: np.ndarray,
    fs: float,
    low: float = 0.5,
    high: float = 45.0,
    order: int = 4,
) -> np.ndarray:
    """Apply a zero-phase Butterworth band-pass using second-order sections."""
    low, high = validate_frequency_range(low, high, fs)
    if order < 1:
        raise ValueError("order must be at least 1")
    x, was_1d = as_continuous(data)
    if np.isnan(x).any():
        raise ValueError("bandpass_filter does not accept NaN values")
    sos = signal.butter(order, [low, high], btype="bandpass", fs=fs, output="sos")
    try:
        y = signal.sosfiltfilt(sos, x, axis=0)
    except ValueError as exc:
        raise ValueError("signal is too short for zero-phase band-pass filtering") from exc
    return restore_channel_shape(y, was_1d)


def lowpass_filter(data: np.ndarray, fs: float, cutoff: float, order: int = 4) -> np.ndarray:
    """Apply a zero-phase Butterworth low-pass filter."""
    fs = validate_sampling_rate(fs)
    if not 0 < cutoff < fs / 2:
        raise ValueError("cutoff must be between 0 and Nyquist")
    x, was_1d = as_continuous(data)
    sos = signal.butter(order, cutoff, btype="lowpass", fs=fs, output="sos")
    y = signal.sosfiltfilt(sos, x, axis=0)
    return restore_channel_shape(y, was_1d)


def highpass_filter(data: np.ndarray, fs: float, cutoff: float, order: int = 4) -> np.ndarray:
    """Apply a zero-phase Butterworth high-pass filter."""
    fs = validate_sampling_rate(fs)
    if not 0 < cutoff < fs / 2:
        raise ValueError("cutoff must be between 0 and Nyquist")
    x, was_1d = as_continuous(data)
    sos = signal.butter(order, cutoff, btype="highpass", fs=fs, output="sos")
    y = signal.sosfiltfilt(sos, x, axis=0)
    return restore_channel_shape(y, was_1d)


def resample(data: np.ndarray, fs: float, new_fs: float, max_denominator: int = 1000) -> np.ndarray:
    """Resample continuous data with polyphase filtering.

    A rational approximation is used so common EEG rates such as 1000 -> 250 Hz
    are handled efficiently.
    """
    fs = validate_sampling_rate(fs)
    new_fs = validate_sampling_rate(new_fs)
    if max_denominator < 1:
        raise ValueError("max_denominator must be positive")
    x, was_1d = as_continuous(data)
    ratio = Fraction(new_fs / fs).limit_denominator(max_denominator)
    y = signal.resample_poly(x, ratio.numerator, ratio.denominator, axis=0)
    return restore_channel_shape(y, was_1d)


def interpolate_bad_samples(data: np.ndarray, bad_mask: np.ndarray) -> np.ndarray:
    """Linearly interpolate explicitly marked bad samples per channel.

    ``bad_mask`` may be shaped ``samples`` (same bad samples for all channels)
    or ``samples x channels``. Edge gaps are filled with the nearest valid
    sample. This function is for short dropouts, not long missing intervals.
    """
    x, was_1d = as_continuous(data, copy=True)
    mask = np.asarray(bad_mask, dtype=bool)
    if mask.ndim == 1:
        if mask.size != x.shape[0]:
            raise ValueError("1-D bad_mask length must match samples")
        mask = np.repeat(mask[:, None], x.shape[1], axis=1)
    if mask.shape != x.shape:
        raise ValueError("bad_mask must match data shape")

    sample_index = np.arange(x.shape[0])
    for channel in range(x.shape[1]):
        bad = mask[:, channel] | ~np.isfinite(x[:, channel])
        good = ~bad
        if np.count_nonzero(good) < 2:
            raise ValueError(f"channel {channel} has fewer than two valid samples")
        x[bad, channel] = np.interp(sample_index[bad], sample_index[good], x[good, channel])
    return restore_channel_shape(x, was_1d)
