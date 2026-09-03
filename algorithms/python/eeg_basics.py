"""Readable EEG analysis helpers for ADS1299-Core-SDK.

Data convention
---------------
- continuous data: samples x channels
- epoched data: epochs x samples x channels
- host-side amplitudes: microvolts (uV) unless the caller documents otherwise

These helpers are intended for education, prototyping and reproducible starter
analyses. For production scientific pipelines, also consider mature packages
such as MNE-Python and validate every processing choice for the experiment.
"""

from __future__ import annotations

from dataclasses import dataclass
from typing import Iterable, Sequence

import numpy as np
from scipy import signal


DEFAULT_BANDS = {
    "delta": (0.5, 4.0),
    "theta": (4.0, 8.0),
    "alpha": (8.0, 13.0),
    "beta": (13.0, 30.0),
    "gamma": (30.0, 45.0),
}


@dataclass
class BandPowerResult:
    absolute: dict[str, np.ndarray]
    relative: dict[str, np.ndarray]


def _as_2d(data: np.ndarray) -> tuple[np.ndarray, bool]:
    """Return samples x channels representation and whether input was 1-D."""
    x = np.asarray(data, dtype=float)
    if x.ndim == 1:
        return x[:, None], True
    if x.ndim != 2:
        raise ValueError("Expected 1-D or 2-D continuous data")
    return x, False


def remove_dc(data: np.ndarray) -> np.ndarray:
    """Subtract each channel mean."""
    x, was_1d = _as_2d(data)
    y = x - np.nanmean(x, axis=0, keepdims=True)
    return y[:, 0] if was_1d else y


def detrend(data: np.ndarray, kind: str = "linear") -> np.ndarray:
    """Remove a constant or linear trend channel-by-channel."""
    x, was_1d = _as_2d(data)
    y = signal.detrend(x, axis=0, type=kind)
    return y[:, 0] if was_1d else y


def common_average_reference(data: np.ndarray) -> np.ndarray:
    """Subtract the instantaneous mean across channels (CAR)."""
    x, was_1d = _as_2d(data)
    if x.shape[1] < 2:
        raise ValueError("CAR requires at least two channels")
    y = x - np.nanmean(x, axis=1, keepdims=True)
    return y[:, 0] if was_1d else y


def rereference(data: np.ndarray, reference_channels: Sequence[int]) -> np.ndarray:
    """Reference all channels to the average of selected reference channels."""
    x, was_1d = _as_2d(data)
    refs = np.asarray(reference_channels, dtype=int)
    if refs.size == 0:
        raise ValueError("reference_channels cannot be empty")
    if np.any(refs < 0) or np.any(refs >= x.shape[1]):
        raise IndexError("reference channel index out of range")
    ref = np.nanmean(x[:, refs], axis=1, keepdims=True)
    y = x - ref
    return y[:, 0] if was_1d else y


def notch_filter(
    data: np.ndarray,
    fs: float,
    freq: float = 50.0,
    q: float = 30.0,
) -> np.ndarray:
    """Zero-phase IIR notch filter, typically for 50/60 Hz mains interference."""
    if not 0 < freq < fs / 2:
        raise ValueError("Notch frequency must be below Nyquist")
    x, was_1d = _as_2d(data)
    b, a = signal.iirnotch(w0=freq, Q=q, fs=fs)
    y = signal.filtfilt(b, a, x, axis=0)
    return y[:, 0] if was_1d else y


def bandpass_filter(
    data: np.ndarray,
    fs: float,
    low: float = 0.5,
    high: float = 45.0,
    order: int = 4,
) -> np.ndarray:
    """Zero-phase Butterworth band-pass filter."""
    if not (0 < low < high < fs / 2):
        raise ValueError("Require 0 < low < high < Nyquist")
    x, was_1d = _as_2d(data)
    sos = signal.butter(order, [low, high], btype="bandpass", fs=fs, output="sos")
    y = signal.sosfiltfilt(sos, x, axis=0)
    return y[:, 0] if was_1d else y


def welch_psd(
    data: np.ndarray,
    fs: float,
    nperseg: int | None = None,
) -> tuple[np.ndarray, np.ndarray]:
    """Welch PSD. Returns frequencies and PSD shaped frequencies x channels."""
    x, _ = _as_2d(data)
    if nperseg is None:
        nperseg = min(x.shape[0], max(32, int(round(2 * fs))))
    freqs, psd = signal.welch(x, fs=fs, axis=0, nperseg=nperseg)
    return freqs, psd


def bandpower(
    data: np.ndarray,
    fs: float,
    bands: dict[str, tuple[float, float]] | None = None,
    total_range: tuple[float, float] = (0.5, 45.0),
    nperseg: int | None = None,
) -> BandPowerResult:
    """Compute absolute and relative band power by integrating Welch PSD."""
    bands = DEFAULT_BANDS if bands is None else bands
    freqs, psd = welch_psd(data, fs=fs, nperseg=nperseg)

    total_mask = (freqs >= total_range[0]) & (freqs <= total_range[1])
    if np.count_nonzero(total_mask) < 2:
        raise ValueError("Insufficient frequency bins in total_range")
    total = np.trapezoid(psd[total_mask], freqs[total_mask], axis=0)

    absolute: dict[str, np.ndarray] = {}
    relative: dict[str, np.ndarray] = {}
    for name, (low, high) in bands.items():
        mask = (freqs >= low) & (freqs < high)
        if np.count_nonzero(mask) < 2:
            power = np.full(psd.shape[1], np.nan)
        else:
            power = np.trapezoid(psd[mask], freqs[mask], axis=0)
        absolute[name] = power
        relative[name] = np.divide(
            power,
            total,
            out=np.full_like(power, np.nan, dtype=float),
            where=total > 0,
        )
    return BandPowerResult(absolute=absolute, relative=relative)


def alpha_peak(
    data: np.ndarray,
    fs: float,
    alpha_range: tuple[float, float] = (8.0, 13.0),
) -> np.ndarray:
    """Return strongest Welch-PSD frequency in alpha_range for each channel."""
    freqs, psd = welch_psd(data, fs)
    mask = (freqs >= alpha_range[0]) & (freqs <= alpha_range[1])
    if not np.any(mask):
        raise ValueError("No PSD bins inside alpha_range")
    f = freqs[mask]
    p = psd[mask]
    return f[np.argmax(p, axis=0)]


def time_domain_features(data: np.ndarray) -> dict[str, np.ndarray]:
    """Basic channel-wise descriptive features."""
    x, _ = _as_2d(data)
    return {
        "mean": np.nanmean(x, axis=0),
        "std": np.nanstd(x, axis=0, ddof=1),
        "rms": np.sqrt(np.nanmean(x**2, axis=0)),
        "peak_to_peak": np.nanmax(x, axis=0) - np.nanmin(x, axis=0),
    }


def hjorth_parameters(data: np.ndarray) -> dict[str, np.ndarray]:
    """Hjorth activity, mobility and complexity for each channel."""
    x, _ = _as_2d(data)
    dx = np.diff(x, axis=0)
    ddx = np.diff(dx, axis=0)

    var0 = np.var(x, axis=0)
    var1 = np.var(dx, axis=0)
    var2 = np.var(ddx, axis=0)

    mobility = np.sqrt(np.divide(var1, var0, out=np.zeros_like(var0), where=var0 > 0))
    mobility_dx = np.sqrt(
        np.divide(var2, var1, out=np.zeros_like(var1), where=var1 > 0)
    )
    complexity = np.divide(
        mobility_dx,
        mobility,
        out=np.zeros_like(mobility),
        where=mobility > 0,
    )
    return {
        "activity": var0,
        "mobility": mobility,
        "complexity": complexity,
    }


def epoch_data(
    data: np.ndarray,
    event_samples: Iterable[int],
    fs: float,
    tmin: float,
    tmax: float,
) -> tuple[np.ndarray, np.ndarray, np.ndarray]:
    """Extract fixed windows around event sample indices.

    Returns
    -------
    epochs : epochs x samples x channels
    times  : seconds relative to event
    kept_events : event sample indices that had complete windows
    """
    x, _ = _as_2d(data)
    start_offset = int(round(tmin * fs))
    stop_offset = int(round(tmax * fs))
    if stop_offset <= start_offset:
        raise ValueError("tmax must be greater than tmin")

    epochs = []
    kept = []
    for event in event_samples:
        start = int(event) + start_offset
        stop = int(event) + stop_offset
        if start >= 0 and stop <= x.shape[0]:
            epochs.append(x[start:stop])
            kept.append(int(event))

    n_times = stop_offset - start_offset
    times = np.arange(n_times) / fs + tmin
    if not epochs:
        return np.empty((0, n_times, x.shape[1])), times, np.asarray([], dtype=int)
    return np.stack(epochs, axis=0), times, np.asarray(kept, dtype=int)


def baseline_correct(
    epochs: np.ndarray,
    times: np.ndarray,
    baseline: tuple[float, float] = (-0.2, 0.0),
) -> np.ndarray:
    """Subtract each epoch/channel baseline mean."""
    ep = np.asarray(epochs, dtype=float)
    if ep.ndim != 3:
        raise ValueError("Expected epochs x samples x channels")
    mask = (times >= baseline[0]) & (times <= baseline[1])
    if not np.any(mask):
        raise ValueError("Baseline interval does not overlap epoch times")
    base = np.nanmean(ep[:, mask, :], axis=1, keepdims=True)
    return ep - base


def reject_epochs_by_amplitude(
    epochs: np.ndarray,
    threshold_uv: float = 150.0,
) -> tuple[np.ndarray, np.ndarray]:
    """Simple educational artifact rejection by absolute amplitude threshold."""
    ep = np.asarray(epochs, dtype=float)
    if ep.ndim != 3:
        raise ValueError("Expected epochs x samples x channels")
    keep = np.nanmax(np.abs(ep), axis=(1, 2)) <= threshold_uv
    return ep[keep], keep


def erp_average(epochs: np.ndarray) -> np.ndarray:
    """Average epochs; output samples x channels."""
    ep = np.asarray(epochs, dtype=float)
    if ep.ndim != 3 or ep.shape[0] == 0:
        raise ValueError("Need at least one epoch")
    return np.nanmean(ep, axis=0)


def magnitude_squared_coherence(
    x: np.ndarray,
    y: np.ndarray,
    fs: float,
    nperseg: int | None = None,
) -> tuple[np.ndarray, np.ndarray]:
    """Magnitude-squared coherence between two 1-D channel signals."""
    a = np.asarray(x, dtype=float).squeeze()
    b = np.asarray(y, dtype=float).squeeze()
    if a.ndim != 1 or b.ndim != 1 or a.shape != b.shape:
        raise ValueError("x and y must be equal-length 1-D arrays")
    if nperseg is None:
        nperseg = min(len(a), max(32, int(round(2 * fs))))
    return signal.coherence(a, b, fs=fs, nperseg=nperseg)
