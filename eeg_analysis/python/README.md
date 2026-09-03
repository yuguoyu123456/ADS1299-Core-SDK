# ADS1299 EEG Python Toolkit

This directory contains the reusable Python EEG-analysis layer for ADS1299-Core-SDK.

It is no longer intended to be a collection of unrelated demo scripts. The main package is `ads1299_eeg/`, which can be installed in editable mode and imported from notebooks, command-line tools, tests, BCI pipelines or future GUI applications.

## Install

From the repository root:

```bash
python -m pip install -e eeg_analysis/python
```

The core package intentionally depends only on NumPy and SciPy. Optional interoperability examples may use MNE-Python or other established research packages separately.

## Data convention

Continuous data:

```text
samples x channels
```

Epoched data:

```text
epochs x samples x channels
```

Host-side EEG amplitudes are normally represented in microvolts unless a function explicitly says that it operates on raw ADC codes or volts.

## Quick start

```python
import numpy as np
from ads1299_eeg import basic_eeg_pipeline

fs = 250.0
seconds = 20
t = np.arange(int(fs * seconds)) / fs

# Eight-channel synthetic EEG with a 10-Hz alpha component.
rng = np.random.default_rng(7)
data = np.column_stack([
    20 * np.sin(2 * np.pi * 10 * t + phase) + rng.normal(0, 4, t.size)
    for phase in np.linspace(0, 1.2, 8)
])

result = basic_eeg_pipeline(
    data,
    fs,
    mains_frequency=50.0,
    low_cut_hz=0.5,
    high_cut_hz=45.0,
)

print(result.quality_after.bad_channel_names)
print(result.bandpower.relative["alpha"])
print(result.features["alpha_peak_hz"])
```

## Package structure

```text
ads1299_eeg/
├── ads1299.py        # signed 24-bit decoding and ADC-code conversion
├── core.py           # shared validation and data conventions
├── preprocessing.py  # reference, filters, resampling, dropout interpolation
├── spectral.py       # Welch PSD, band power, alpha peak, entropy, line noise
├── quality.py        # channel-quality screening and explicit failure reasons
├── features.py       # time-domain, Hjorth and spectral feature tables
├── epochs.py         # epoch extraction, baseline correction, ERP helpers
├── connectivity.py   # correlation, coherence and PLV starter methods
├── io.py             # lightweight CSV import/export
└── pipeline.py       # transparent starter processing pipeline
```

## ADS1299 code conversion

The ADS1299 produces signed 24-bit two's-complement codes. The package provides:

```python
from ads1299_eeg import decode_24be, codes_to_microvolts

code = decode_24be(bytes.fromhex("000001"))
uv = codes_to_microvolts([code], vref=4.5, gain=24)
```

`vref` and `gain` are explicit because a host application must not silently assume the wrong analog configuration.

## Preprocessing

Available building blocks include:

- DC removal and detrending;
- common-average reference;
- selected-channel rereference;
- bipolar derivations;
- 50/60-Hz notch filtering and optional harmonics;
- Butterworth high-pass, low-pass and band-pass filtering;
- polyphase resampling;
- explicit short-gap interpolation.

The default functions use zero-phase offline filters where appropriate. Online/causal filtering is a separate problem and should not silently reuse an offline zero-phase pipeline.

## Spectral analysis

Available functions include:

- Welch PSD;
- absolute and relative delta/theta/alpha/beta/gamma power;
- dominant frequency;
- alpha peak frequency;
- normalized spectral entropy;
- line-noise power ratio;
- relative spectra.

## Signal-quality report

`assess_quality()` provides a transparent engineering screen per channel:

- mean, standard deviation and RMS;
- peak-to-peak amplitude;
- non-finite sample fraction;
- flatline fraction;
- clipping fraction;
- line-noise ratio;
- correlation with the median waveform;
- explicit bad-channel reasons.

The thresholds are starting points, not clinical rules. Electrode type, montage, environment and experiment must determine the final acceptance criteria.

## ERP workflow

A typical event-related workflow is:

```python
from ads1299_eeg import (
    epoch_data,
    baseline_correct,
    reject_epochs_by_amplitude,
    erp_average,
    peak_in_window,
)

epochs, times, kept = epoch_data(data, event_samples, fs, -0.2, 0.8)
epochs = baseline_correct(epochs, times, (-0.2, 0.0))
epochs, keep_mask, diagnostics = reject_epochs_by_amplitude(
    epochs,
    absolute_threshold_uv=150,
)
erp = erp_average(epochs)
amplitude, latency = peak_in_window(erp, times, (0.25, 0.5), polarity="positive")
```

## Connectivity

Starter connectivity functions include Pearson correlation, band-limited magnitude-squared coherence and phase-locking value. Connectivity measures are highly sensitive to volume conduction, referencing and preprocessing; these functions are educational/research building blocks rather than automatic physiological interpretations.

## Testing policy

The Python package must pass synthetic-signal tests before changes are merged. Tests check numerical properties such as:

- signed 24-bit conversion;
- code-to-voltage round trips;
- notch attenuation;
- alpha peak detection;
- band-power dominance;
- epoch extraction and baseline correction;
- signal-quality detection;
- connectivity matrix shape/symmetry;
- CSV round trip;
- complete pipeline execution.

Real EEG examples will be added separately after the corresponding hardware/data provenance is documented.

## Relationship to mature EEG ecosystems

This toolkit is designed to make ADS1299-specific engineering and beginner analysis readable in one repository. It does not attempt to replace mature research ecosystems. Where appropriate, the project will provide interoperability paths for software such as MNE-Python, EEGLAB and FieldTrip rather than copying large third-party implementations into this repository.
