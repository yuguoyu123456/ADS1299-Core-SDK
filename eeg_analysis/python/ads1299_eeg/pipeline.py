"""Composable starter pipelines for ADS1299 EEG recordings."""

from __future__ import annotations

from dataclasses import dataclass

import numpy as np

from .features import extract_feature_table
from .preprocessing import bandpass_filter, common_average_reference, notch_filter, remove_dc
from .quality import QualityReport, assess_quality
from .spectral import BandPowerResult, bandpower


@dataclass(frozen=True)
class BasicPipelineResult:
    raw: np.ndarray
    processed: np.ndarray
    quality_before: QualityReport
    quality_after: QualityReport
    bandpower: BandPowerResult
    features: dict[str, np.ndarray]
    fs: float


def basic_eeg_pipeline(
    data: np.ndarray,
    fs: float,
    *,
    mains_frequency: float = 50.0,
    low_cut_hz: float = 0.5,
    high_cut_hz: float = 45.0,
    notch_q: float = 30.0,
    notch_harmonics: int = 1,
    apply_car: bool = False,
    channel_names: list[str] | tuple[str, ...] | None = None,
) -> BasicPipelineResult:
    """Run a transparent baseline EEG processing workflow.

    Processing order:

    1. quality report on original data;
    2. remove per-channel DC mean;
    3. notch mains interference when below Nyquist;
    4. band-pass to the requested analysis band;
    5. optional common-average reference;
    6. post-processing quality report;
    7. PSD/band-power and reusable feature extraction.

    This is a starter workflow, not a universal scientific prescription.
    Experiment-specific preprocessing should be documented and validated.
    """
    raw = np.asarray(data, dtype=float)
    quality_before = assess_quality(
        raw,
        fs,
        channel_names=channel_names,
        mains_frequency=mains_frequency,
    )

    processed = remove_dc(raw)
    if mains_frequency < fs / 2:
        processed = notch_filter(
            processed,
            fs,
            freq=mains_frequency,
            q=notch_q,
            harmonics=notch_harmonics,
        )
    processed = bandpass_filter(processed, fs, low=low_cut_hz, high=high_cut_hz)
    if apply_car:
        processed = common_average_reference(processed)

    quality_after = assess_quality(
        processed,
        fs,
        channel_names=channel_names,
        mains_frequency=mains_frequency,
    )
    powers = bandpower(processed, fs)
    features = extract_feature_table(processed, fs)
    return BasicPipelineResult(
        raw=raw,
        processed=np.asarray(processed),
        quality_before=quality_before,
        quality_after=quality_after,
        bandpower=powers,
        features=features,
        fs=float(fs),
    )
