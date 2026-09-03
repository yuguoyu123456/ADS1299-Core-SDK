"""Signal-quality metrics for continuous EEG.

These metrics are intentionally transparent. They do not replace electrode
impedance measurements, lead-off detection or domain-specific artifact review.
They provide a reproducible first-pass screen for recordings and integration
tests.
"""

from __future__ import annotations

from dataclasses import dataclass

import numpy as np

from .core import as_continuous, default_channel_names, validate_sampling_rate
from .spectral import line_noise_ratio


@dataclass(frozen=True)
class ChannelQuality:
    name: str
    mean_uv: float
    std_uv: float
    rms_uv: float
    peak_to_peak_uv: float
    flatline_fraction: float
    nonfinite_fraction: float
    clipping_fraction: float
    line_noise_ratio: float
    correlation_to_median: float
    bad: bool
    reasons: tuple[str, ...]


@dataclass(frozen=True)
class QualityReport:
    sampling_rate_hz: float
    n_samples: int
    duration_seconds: float
    channels: tuple[ChannelQuality, ...]

    @property
    def bad_channel_indices(self) -> tuple[int, ...]:
        return tuple(index for index, channel in enumerate(self.channels) if channel.bad)

    @property
    def bad_channel_names(self) -> tuple[str, ...]:
        return tuple(channel.name for channel in self.channels if channel.bad)


def _flatline_fraction(channel: np.ndarray, tolerance_uv: float) -> float:
    if channel.size < 2:
        return 1.0
    finite = np.isfinite(channel[:-1]) & np.isfinite(channel[1:])
    if not np.any(finite):
        return 1.0
    differences = np.abs(np.diff(channel))
    return float(np.mean(differences[finite] <= tolerance_uv))


def _clipping_fraction(channel: np.ndarray, clipping_uv: float | None) -> float:
    if clipping_uv is None:
        return 0.0
    finite = np.isfinite(channel)
    if not np.any(finite):
        return 1.0
    return float(np.mean(np.abs(channel[finite]) >= clipping_uv))


def _correlation_to_median(data: np.ndarray) -> np.ndarray:
    """Correlation of each channel with a robust median reference waveform."""
    x = np.asarray(data, dtype=float)
    result = np.full(x.shape[1], np.nan, dtype=float)
    reference = np.nanmedian(x, axis=1)
    reference_std = np.nanstd(reference)
    if not np.isfinite(reference_std) or reference_std == 0:
        return result
    for channel in range(x.shape[1]):
        valid = np.isfinite(x[:, channel]) & np.isfinite(reference)
        if np.count_nonzero(valid) < 3:
            continue
        current = x[valid, channel]
        ref = reference[valid]
        if np.std(current) == 0 or np.std(ref) == 0:
            continue
        result[channel] = float(np.corrcoef(current, ref)[0, 1])
    return result


def assess_quality(
    data: np.ndarray,
    fs: float,
    *,
    channel_names: list[str] | tuple[str, ...] | None = None,
    mains_frequency: float = 50.0,
    flatline_tolerance_uv: float = 0.02,
    max_flatline_fraction: float = 0.20,
    clipping_uv: float | None = 5000.0,
    max_clipping_fraction: float = 0.001,
    max_nonfinite_fraction: float = 0.0,
    max_line_noise_ratio: float = 0.25,
    max_peak_to_peak_uv: float = 2000.0,
    min_std_uv: float = 0.05,
) -> QualityReport:
    """Generate a channel-wise engineering quality report.

    Defaults are deliberately generic and should be adapted to the electrode,
    montage and experiment. A channel is marked bad when one or more explicit
    thresholds are exceeded.
    """
    fs = validate_sampling_rate(fs)
    x, _ = as_continuous(data)
    if channel_names is None:
        names = default_channel_names(x.shape[1])
    else:
        names = list(channel_names)
        if len(names) != x.shape[1]:
            raise ValueError("channel_names length must match channel count")

    finite_x = np.where(np.isfinite(x), x, np.nan)
    correlations = _correlation_to_median(finite_x)

    # Spectral metrics need finite data. Replace isolated nonfinite samples with
    # per-channel medians only for this diagnostic calculation; the report still
    # records the original nonfinite fraction.
    spectral_input = finite_x.copy()
    for channel in range(x.shape[1]):
        finite = np.isfinite(spectral_input[:, channel])
        if np.any(finite):
            fill = float(np.nanmedian(spectral_input[:, channel]))
            spectral_input[~finite, channel] = fill
        else:
            spectral_input[:, channel] = 0.0
    try:
        line_ratios = line_noise_ratio(
            spectral_input,
            fs,
            mains_frequency=mains_frequency,
            reference_band=(1.0, min(100.0, fs / 2 - 0.01)),
        )
    except ValueError:
        line_ratios = np.full(x.shape[1], np.nan)

    channels: list[ChannelQuality] = []
    for index, name in enumerate(names):
        channel = x[:, index]
        finite = np.isfinite(channel)
        nonfinite_fraction = float(1.0 - np.mean(finite))
        finite_values = channel[finite]
        reasons: list[str] = []

        if finite_values.size:
            mean = float(np.mean(finite_values))
            std = float(np.std(finite_values, ddof=1)) if finite_values.size > 1 else 0.0
            rms = float(np.sqrt(np.mean(finite_values**2)))
            ptp = float(np.ptp(finite_values))
        else:
            mean = std = rms = ptp = float("nan")

        flatline = _flatline_fraction(channel, flatline_tolerance_uv)
        clipping = _clipping_fraction(channel, clipping_uv)
        line_ratio = float(line_ratios[index])
        correlation = float(correlations[index])

        if nonfinite_fraction > max_nonfinite_fraction:
            reasons.append(f"nonfinite>{max_nonfinite_fraction:.3g}")
        if flatline > max_flatline_fraction:
            reasons.append(f"flatline>{max_flatline_fraction:.3g}")
        if clipping > max_clipping_fraction:
            reasons.append(f"clipping>{max_clipping_fraction:.3g}")
        if np.isfinite(line_ratio) and line_ratio > max_line_noise_ratio:
            reasons.append(f"line_noise>{max_line_noise_ratio:.3g}")
        if np.isfinite(ptp) and ptp > max_peak_to_peak_uv:
            reasons.append(f"peak_to_peak>{max_peak_to_peak_uv:.3g}uV")
        if np.isfinite(std) and std < min_std_uv:
            reasons.append(f"std<{min_std_uv:.3g}uV")
        if finite_values.size == 0:
            reasons.append("no_finite_samples")

        channels.append(
            ChannelQuality(
                name=name,
                mean_uv=mean,
                std_uv=std,
                rms_uv=rms,
                peak_to_peak_uv=ptp,
                flatline_fraction=flatline,
                nonfinite_fraction=nonfinite_fraction,
                clipping_fraction=clipping,
                line_noise_ratio=line_ratio,
                correlation_to_median=correlation,
                bad=bool(reasons),
                reasons=tuple(reasons),
            )
        )

    return QualityReport(
        sampling_rate_hz=fs,
        n_samples=x.shape[0],
        duration_seconds=x.shape[0] / fs,
        channels=tuple(channels),
    )


def report_as_rows(report: QualityReport) -> list[dict[str, object]]:
    """Convert a quality report to plain dictionaries for CSV/JSON export."""
    rows: list[dict[str, object]] = []
    for channel in report.channels:
        rows.append(
            {
                "channel": channel.name,
                "mean_uv": channel.mean_uv,
                "std_uv": channel.std_uv,
                "rms_uv": channel.rms_uv,
                "peak_to_peak_uv": channel.peak_to_peak_uv,
                "flatline_fraction": channel.flatline_fraction,
                "nonfinite_fraction": channel.nonfinite_fraction,
                "clipping_fraction": channel.clipping_fraction,
                "line_noise_ratio": channel.line_noise_ratio,
                "correlation_to_median": channel.correlation_to_median,
                "bad": channel.bad,
                "reasons": ";".join(channel.reasons),
            }
        )
    return rows
