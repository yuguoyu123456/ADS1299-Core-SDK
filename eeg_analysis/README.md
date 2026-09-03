# EEG Algorithms

This directory contains beginner-friendly, transparent EEG analysis examples that operate on multichannel sample matrices. The code is intentionally kept readable so users can understand what happens between raw ADS1299 samples and derived EEG features.

## Recommended learning order

1. **Import and units** — confirm sampling rate, channel order and volts/µV scaling.
2. **Preprocessing** — detrend, rereference, notch, band-pass.
3. **Quality control** — inspect saturation, flat channels, large-amplitude artifacts and line noise.
4. **Spectral analysis** — PSD, band power, alpha peak.
5. **Epoch/event analysis** — epoching, baseline correction, ERP.
6. **Connectivity/features** — coherence, Hjorth and other interpretable features.
7. **Advanced BCI methods** — CSP, CCA/FBCCA, xDAWN, Riemannian features and ML.

## Folder layout

```text
algorithms/
├── python/
│   ├── eeg_basics.py
│   ├── demo_basic_analysis.py
│   └── requirements.txt
├── matlab/
│   └── eeg_basic_analysis.m
├── R/
│   └── eeg_basic_analysis.R
└── ROADMAP.md
```

## Core methods included in the first bootstrap

### Preprocessing
- mean/DC removal
- linear detrending
- common average reference (CAR)
- single-reference rereferencing
- IIR notch filtering
- Butterworth band-pass filtering

### Spectral
- Welch power spectral density (PSD)
- delta/theta/alpha/beta/gamma power
- relative band power
- individual alpha peak estimation

### Time-domain
- mean, standard deviation, RMS
- peak-to-peak amplitude
- Hjorth activity, mobility and complexity

### Event-related analysis
- fixed-window epoch extraction
- baseline correction
- simple artifact rejection
- ERP averaging

### Connectivity
- magnitude-squared coherence

## Advanced roadmap

The roadmap intentionally separates methods that can be implemented locally from mature third-party ecosystems that should usually be used directly instead of copied. Planned integrations/examples include:

- **Python:** MNE-Python, SciPy, NumPy, pandas, scikit-learn, pyRiemann, MOABB
- **MATLAB:** EEGLAB, FieldTrip, BCILAB-style workflows where licensing allows
- **R:** signal, eegUtils/eegkit-related workflows where appropriate

Advanced algorithm topics:

- ICA artifact decomposition
- ASR / robust artifact handling
- ERD/ERS time-frequency maps
- STFT / Morlet wavelets
- CSP / FBCSP for motor imagery
- CCA / FBCCA for SSVEP
- xDAWN / P300 pipelines
- LDA / logistic regression / SVM baselines
- Riemannian covariance features
- PLV / PLI / wPLI / coherence networks
- PAC and cross-frequency coupling
- microstate analysis
- sleep-stage feature examples
- quality metrics and channel-noise scoring

## Data convention

Unless a file says otherwise:

- arrays are shaped as **samples × channels**
- sample values are in **µV** for host-side examples
- sampling frequency is passed explicitly as `fs`
- channel labels are metadata, not hard-coded into the algorithms

## Important note

These examples are educational/research code. Filter choices, rejection thresholds and frequency bands must be adapted to the experiment. Do not treat default values as a clinical standard.
