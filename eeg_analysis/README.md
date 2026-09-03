# EEG Analysis

This folder contains beginner-friendly EEG analysis workflows that operate above the ADS1299 acquisition layer. The emphasis is on readable code, explicit units and reproducible steps from raw multichannel samples to interpretable EEG features.

## Where should I start?

- **Python** → [`python/`](python/)
- **MATLAB** → [`matlab/`](matlab/)
- **R** → [`r/`](r/)
- **BCI algorithms** → [`../bci/`](../bci/)

The same scientific concepts should remain recognizable across languages even when the exact library APIs differ.

## Recommended learning order

1. **Import and units** — confirm sampling rate, channel order and volts/µV scaling.
2. **Preprocessing** — remove DC/trend, rereference, notch and band-pass.
3. **Quality control** — inspect saturation, flat channels, large artifacts and line noise.
4. **Spectral analysis** — PSD, band power and alpha peak.
5. **Epoch/event analysis** — epoching, baseline correction and ERP.
6. **Connectivity and features** — coherence, Hjorth and other interpretable features.
7. **BCI** — continue to SSVEP, motor imagery, P300 and online pipelines in `../bci/`.

## Current language layout

```text
eeg_analysis/
├── python/
│   ├── eeg_basics.py
│   ├── demo_basic_analysis.py
│   ├── mne_quickstart.py
│   └── requirements.txt
├── matlab/
│   ├── eeg_basic_analysis.m
│   ├── p300_erp_baseline.m
│   └── ssvep_cca_baseline.m
├── r/
│   ├── eeg_basic_analysis.R
│   ├── p300_erp_baseline.R
│   └── ssvep_cca_baseline.R
└── ROADMAP.md
```

## Core topics

### Preprocessing
- mean / DC removal;
- linear detrending;
- common-average reference (CAR);
- single-reference rereferencing;
- notch filtering;
- band-pass filtering.

### Frequency domain
- FFT concepts;
- Welch power spectral density (PSD);
- delta / theta / alpha / beta / gamma power;
- relative band power;
- individual alpha peak estimation.

### Time domain
- mean, standard deviation and RMS;
- peak-to-peak amplitude;
- Hjorth activity, mobility and complexity.

### Event-related analysis
- fixed-window epoch extraction;
- baseline correction;
- simple artifact rejection;
- ERP averaging;
- introductory P300 measurements.

### Connectivity
- magnitude-squared coherence;
- roadmap for phase- and network-based methods.

## Established ecosystems

This repository should interoperate with mature scientific ecosystems rather than duplicate them unnecessarily.

- **Python:** NumPy, SciPy, pandas, MNE-Python, scikit-learn, pyRiemann, MOABB.
- **MATLAB:** EEGLAB, FieldTrip and compatible research workflows.
- **R:** signal-processing and EEG/statistical packages where appropriate.

Third-party libraries and datasets must keep their own licenses and citation requirements.

## Data convention

Unless an example explicitly says otherwise:

- arrays are **samples × channels**;
- host-side EEG values are expressed in **µV**;
- sampling frequency is passed explicitly as `fs`;
- channel labels are metadata rather than hidden assumptions;
- filtering and artifact thresholds are examples, not universal clinical standards.

## Validation

Analysis examples should state what has actually been verified: synthetic-data checks, named public datasets, ADS1299-Core hardware data, or online acquisition. Educational code must not be presented as clinically validated software.
