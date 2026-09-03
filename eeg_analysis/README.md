# EEG Analysis

This folder is the signal-analysis layer above ADS1299 acquisition. The goal is to provide a readable path from raw ADC samples to reproducible EEG quality checks, preprocessing, spectral analysis, ERP workflows, connectivity, features and later BCI pipelines.

## Where should I start?

- **Python production toolkit** → [`python/`](python/)
- **MATLAB workflows** → [`matlab/`](matlab/)
- **R workflows** → [`r/`](r/)
- **BCI algorithms** → [`../bci/`](../bci/)

The same scientific concepts should remain recognizable across languages even when the exact APIs differ.

## Python is now an installable package

The Python track is no longer only a set of scripts. From the repository root:

```bash
python -m pip install -e eeg_analysis/python
```

Then:

```python
from ads1299_eeg import basic_eeg_pipeline, bandpower, assess_quality
```

The reusable package currently contains:

```text
eeg_analysis/python/
├── pyproject.toml
├── README.md
├── requirements.txt
├── ads1299_eeg/
│   ├── __init__.py
│   ├── ads1299.py        # signed 24-bit decoding and ADC-code scaling
│   ├── core.py           # data conventions and validation
│   ├── preprocessing.py  # reference, filters, resampling, interpolation
│   ├── spectral.py       # Welch PSD, band power, peaks, entropy, line noise
│   ├── quality.py        # channel-quality report and explicit bad reasons
│   ├── features.py       # time-domain, Hjorth and spectral features
│   ├── epochs.py         # epoching, baseline, rejection, ERP and peak latency
│   ├── connectivity.py   # correlation, coherence and PLV
│   ├── io.py             # lightweight CSV import/export
│   └── pipeline.py       # transparent starter processing pipeline
├── demo_basic_analysis.py
├── eeg_basics.py         # legacy readable helper file retained for compatibility
└── mne_quickstart.py
```

Numerical regression tests are in `tests/python/test_eeg_package.py`, and the package is built/tested on multiple Python versions through GitHub Actions.

## Recommended learning order

1. **Import and units** — confirm sampling rate, channel order and ADS1299 code-to-voltage scaling.
2. **Signal quality** — identify flat channels, clipping, non-finite samples and strong line noise before interpreting physiology.
3. **Preprocessing** — remove DC/trend, rereference, notch and band-pass.
4. **Spectral analysis** — PSD, band power, spectral entropy and alpha peak.
5. **Epoch/event analysis** — epoching, baseline correction, rejection and ERP.
6. **Connectivity and features** — correlation, coherence, PLV, Hjorth and interpretable features.
7. **BCI** — continue to SSVEP, motor imagery, P300 and online pipelines in `../bci/`.

## Core topics

### ADS1299 host conversion
- 24-bit two's-complement decoding;
- explicit VREF/PGA gain scaling;
- ADC codes to volts and microvolts;
- simulation round trips for test data.

### Preprocessing
- DC removal and detrending;
- common-average reference (CAR);
- selected reference channels;
- bipolar derivations;
- 50/60-Hz notch and harmonics;
- high-pass, low-pass and band-pass filtering;
- polyphase resampling;
- explicit short-dropout interpolation.

### Signal quality
- RMS and peak-to-peak amplitude;
- flatline fraction;
- clipping fraction;
- non-finite sample fraction;
- line-noise power ratio;
- channel-to-median correlation;
- explicit bad-channel reasons.

### Frequency domain
- Welch power spectral density (PSD);
- delta / theta / alpha / beta / gamma power;
- relative band power;
- alpha/dominant peak frequency;
- spectral entropy;
- line-noise ratio.

### Time domain and features
- mean, standard deviation, variance and RMS;
- peak-to-peak and mean absolute amplitude;
- skewness and kurtosis;
- Hjorth activity, mobility and complexity;
- line length;
- zero-crossing and slope-sign-change features;
- flat feature dictionaries for downstream machine learning.

### Event-related analysis
- fixed-window epoch extraction;
- baseline correction;
- amplitude/channel-fraction rejection;
- ERP mean/median;
- ERP SEM;
- positive/negative/absolute peak amplitude and latency.

### Connectivity
- Pearson correlation;
- band-limited magnitude-squared coherence;
- phase-locking value (PLV).

Connectivity values require careful interpretation because referencing, volume conduction and preprocessing can dominate the result.

## MATLAB and R

MATLAB and R currently contain smaller starter implementations. They will be expanded in the same sequence as the Python track: reusable functions first, documented demos second, then automated or reproducible numerical validation where the toolchain allows it.

## Established ecosystems

ADS1299-Core-SDK should interoperate with mature scientific ecosystems rather than disguise copied third-party implementations as project code.

- **Python:** NumPy, SciPy, MNE-Python, scikit-learn, pyRiemann, MOABB and related tools where appropriate.
- **MATLAB:** EEGLAB, FieldTrip and compatible research workflows.
- **R:** established signal-processing/statistical packages where they add value.

Third-party software, code and datasets keep their own licenses, notices and citation requirements. When this project implements an algorithm independently, the implementation should remain readable and testable against the underlying published method rather than mimicking another repository line-for-line.

## Data convention

Unless an example explicitly says otherwise:

- continuous arrays are **samples × channels**;
- epoched arrays are **epochs × samples × channels**;
- host-side EEG amplitudes are expressed in **µV**;
- sampling frequency is passed explicitly as `fs`;
- channel labels are metadata rather than hidden assumptions;
- filter and artifact thresholds are engineering/research starting points, not universal clinical standards.

## Validation

Analysis code should state what has actually been verified: synthetic-data numerical tests, named public datasets, ADS1299-Core hardware data, or online acquisition. Educational/research utilities are not presented as clinically validated software.
