"""ADS1299 EEG analysis toolkit.

The public API intentionally exposes readable building blocks rather than a
single opaque pipeline. Users can start with :func:`basic_eeg_pipeline` and
later assemble experiment-specific workflows from the lower-level functions.
"""

from .ads1299 import (
    ADS1299_BITS,
    ADS1299_CODE_MAX,
    ADS1299_CODE_MIN,
    codes_to_microvolts,
    codes_to_volts,
    decode_24be,
    decode_channels_24be,
    lsb_volts,
    sign_extend_24,
    volts_to_codes,
)
from .connectivity import coherence_matrix, correlation_matrix, phase_locking_value
from .core import DEFAULT_EEG_BANDS, default_channel_names
from .epochs import (
    baseline_correct,
    epoch_data,
    erp_average,
    erp_sem,
    peak_in_window,
    reject_epochs_by_amplitude,
    reject_epochs_by_channel_fraction,
)
from .features import (
    extract_feature_table,
    hjorth_parameters,
    line_length,
    slope_sign_changes,
    time_domain_features,
    zero_crossing_rate,
)
from .io import ContinuousRecording, infer_uniform_sampling_rate, load_channel_csv, save_channel_csv
from .pipeline import BasicPipelineResult, basic_eeg_pipeline
from .preprocessing import (
    bandpass_filter,
    bipolar_reference,
    common_average_reference,
    detrend,
    highpass_filter,
    interpolate_bad_samples,
    lowpass_filter,
    notch_filter,
    remove_dc,
    rereference,
    resample,
)
from .quality import ChannelQuality, QualityReport, assess_quality, report_as_rows
from .spectral import (
    BandPowerResult,
    alpha_peak_frequency,
    bandpower,
    dominant_frequency,
    line_noise_ratio,
    relative_spectrum,
    spectral_entropy,
    welch_psd,
)

__all__ = [
    "ADS1299_BITS",
    "ADS1299_CODE_MAX",
    "ADS1299_CODE_MIN",
    "BandPowerResult",
    "BasicPipelineResult",
    "ChannelQuality",
    "ContinuousRecording",
    "DEFAULT_EEG_BANDS",
    "QualityReport",
    "alpha_peak_frequency",
    "assess_quality",
    "bandpass_filter",
    "bandpower",
    "baseline_correct",
    "basic_eeg_pipeline",
    "bipolar_reference",
    "codes_to_microvolts",
    "codes_to_volts",
    "coherence_matrix",
    "common_average_reference",
    "correlation_matrix",
    "decode_24be",
    "decode_channels_24be",
    "default_channel_names",
    "detrend",
    "dominant_frequency",
    "epoch_data",
    "erp_average",
    "erp_sem",
    "extract_feature_table",
    "highpass_filter",
    "hjorth_parameters",
    "infer_uniform_sampling_rate",
    "interpolate_bad_samples",
    "line_length",
    "line_noise_ratio",
    "load_channel_csv",
    "lowpass_filter",
    "lsb_volts",
    "notch_filter",
    "peak_in_window",
    "phase_locking_value",
    "relative_spectrum",
    "remove_dc",
    "report_as_rows",
    "rereference",
    "reject_epochs_by_amplitude",
    "reject_epochs_by_channel_fraction",
    "resample",
    "save_channel_csv",
    "sign_extend_24",
    "slope_sign_changes",
    "spectral_entropy",
    "time_domain_features",
    "volts_to_codes",
    "welch_psd",
    "zero_crossing_rate",
]

__version__ = "0.1.0"
