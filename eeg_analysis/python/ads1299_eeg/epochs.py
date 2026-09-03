"""Epoch extraction, artifact screening and ERP helpers."""

from __future__ import annotations

from collections.abc import Iterable

import numpy as np

from .core import as_continuous, as_epochs, validate_sampling_rate


def epoch_data(
    data: np.ndarray,
    event_samples: Iterable[int],
    fs: float,
    tmin: float,
    tmax: float,
) -> tuple[np.ndarray, np.ndarray, np.ndarray]:
    """Extract complete fixed-length epochs around event sample indices.

    Returns
    -------
    epochs:
        Array shaped ``epochs x samples x channels``.
    times:
        Seconds relative to each event.
    kept_events:
        Event sample indices for which the complete window was available.
    """
    fs = validate_sampling_rate(fs)
    x, _ = as_continuous(data)
    start_offset = int(round(float(tmin) * fs))
    stop_offset = int(round(float(tmax) * fs))
    if stop_offset <= start_offset:
        raise ValueError("tmax must be greater than tmin")

    windows: list[np.ndarray] = []
    kept: list[int] = []
    for event in event_samples:
        event = int(event)
        start = event + start_offset
        stop = event + stop_offset
        if start >= 0 and stop <= x.shape[0]:
            windows.append(x[start:stop].copy())
            kept.append(event)

    n_times = stop_offset - start_offset
    times = np.arange(n_times, dtype=float) / fs + float(tmin)
    if not windows:
        return (
            np.empty((0, n_times, x.shape[1]), dtype=float),
            times,
            np.asarray([], dtype=int),
        )
    return np.stack(windows, axis=0), times, np.asarray(kept, dtype=int)


def baseline_correct(
    epochs: np.ndarray,
    times: np.ndarray,
    baseline: tuple[float, float] = (-0.2, 0.0),
) -> np.ndarray:
    """Subtract epoch/channel mean over the selected baseline interval."""
    ep = as_epochs(epochs)
    times = np.asarray(times, dtype=float)
    if times.ndim != 1 or times.size != ep.shape[1]:
        raise ValueError("times must be 1-D and match epoch sample count")
    low, high = baseline
    if high < low:
        raise ValueError("baseline high must be >= baseline low")
    mask = (times >= low) & (times <= high)
    if not np.any(mask):
        raise ValueError("baseline interval does not overlap epoch times")
    base = np.nanmean(ep[:, mask, :], axis=1, keepdims=True)
    return ep - base


def reject_epochs_by_amplitude(
    epochs: np.ndarray,
    *,
    absolute_threshold_uv: float = 150.0,
    peak_to_peak_threshold_uv: float | None = None,
) -> tuple[np.ndarray, np.ndarray, dict[str, np.ndarray]]:
    """Reject epochs with transparent amplitude rules.

    Returns the kept epochs, a boolean keep mask and diagnostic arrays.
    """
    ep = as_epochs(epochs)
    if absolute_threshold_uv <= 0:
        raise ValueError("absolute_threshold_uv must be positive")
    absolute_peak = np.nanmax(np.abs(ep), axis=(1, 2))
    keep = absolute_peak <= absolute_threshold_uv

    per_epoch_ptp = np.nanmax(ep, axis=(1, 2)) - np.nanmin(ep, axis=(1, 2))
    if peak_to_peak_threshold_uv is not None:
        if peak_to_peak_threshold_uv <= 0:
            raise ValueError("peak_to_peak_threshold_uv must be positive")
        keep &= per_epoch_ptp <= peak_to_peak_threshold_uv

    diagnostics = {
        "absolute_peak_uv": absolute_peak,
        "peak_to_peak_uv": per_epoch_ptp,
    }
    return ep[keep], keep, diagnostics


def reject_epochs_by_channel_fraction(
    epochs: np.ndarray,
    *,
    threshold_uv: float = 150.0,
    max_bad_channel_fraction: float = 0.25,
) -> tuple[np.ndarray, np.ndarray, np.ndarray]:
    """Reject epochs when too many channels exceed an amplitude threshold."""
    ep = as_epochs(epochs)
    if threshold_uv <= 0:
        raise ValueError("threshold_uv must be positive")
    if not 0 <= max_bad_channel_fraction <= 1:
        raise ValueError("max_bad_channel_fraction must be between 0 and 1")
    bad_channel = np.nanmax(np.abs(ep), axis=1) > threshold_uv
    bad_fraction = np.mean(bad_channel, axis=1)
    keep = bad_fraction <= max_bad_channel_fraction
    return ep[keep], keep, bad_fraction


def erp_average(epochs: np.ndarray, *, robust: bool = False) -> np.ndarray:
    """Return samples x channels ERP using mean or median across epochs."""
    ep = as_epochs(epochs)
    return np.nanmedian(ep, axis=0) if robust else np.nanmean(ep, axis=0)


def erp_sem(epochs: np.ndarray) -> np.ndarray:
    """Standard error of the mean ERP, shaped samples x channels."""
    ep = as_epochs(epochs)
    valid_count = np.sum(np.isfinite(ep), axis=0)
    std = np.nanstd(ep, axis=0, ddof=1)
    return np.divide(
        std,
        np.sqrt(valid_count),
        out=np.full_like(std, np.nan, dtype=float),
        where=valid_count > 1,
    )


def peak_in_window(
    waveform: np.ndarray,
    times: np.ndarray,
    window: tuple[float, float],
    *,
    polarity: str = "positive",
) -> tuple[np.ndarray, np.ndarray]:
    """Return peak amplitude and latency for each channel within a time window."""
    x, _ = as_continuous(waveform)
    times = np.asarray(times, dtype=float)
    if times.ndim != 1 or times.size != x.shape[0]:
        raise ValueError("times must match waveform samples")
    low, high = window
    mask = (times >= low) & (times <= high)
    if not np.any(mask):
        raise ValueError("window does not overlap times")
    local = x[mask]
    local_times = times[mask]
    if polarity == "positive":
        indices = np.nanargmax(local, axis=0)
    elif polarity == "negative":
        indices = np.nanargmin(local, axis=0)
    elif polarity == "absolute":
        indices = np.nanargmax(np.abs(local), axis=0)
    else:
        raise ValueError("polarity must be positive, negative or absolute")
    amplitude = local[indices, np.arange(local.shape[1])]
    latency = local_times[indices]
    return amplitude, latency
