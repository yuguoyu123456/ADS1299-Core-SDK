# ADS1299 EEG MATLAB / GNU Octave Toolkit

This directory is the MATLAB/Octave EEG-analysis layer of ADS1299-Core-SDK.

The reusable implementation lives in the MATLAB namespace package:

```text
+ads1299eeg/
```

This avoids a flat directory full of unrelated scripts and gives users a stable API such as:

```matlab
result = ads1299eeg.basic_pipeline(eeg_uV, 250);
```

## Requirements

### MATLAB

- MATLAB with the Signal Processing Toolbox for Butterworth filtering and `filtfilt`.

### GNU Octave

- GNU Octave
- Octave `signal` package

The numerical CI uses GNU Octave so the public repository can automatically execute the core MATLAB-compatible analysis functions without depending on a proprietary MATLAB license.

## Quick start

From the repository root:

```matlab
addpath('eeg_analysis/matlab');

fs = 250;
t = (0:fs*20-1)'/fs;
data = zeros(numel(t),8);
for ch = 1:8
    data(:,ch) = 20*sin(2*pi*10*t + ch*0.1) + 3*randn(size(t));
end

options = struct('mainsHz',50,'lowHz',0.5,'highHz',45);
result = ads1299eeg.basic_pipeline(data,fs,options);

disp(result.alphaPeakHz);
disp(result.bandpower.relative.alpha);
```

In GNU Octave, load the signal package first:

```octave
pkg load signal
addpath('eeg_analysis/matlab');
```

## Current reusable API

### ADS1299 conversion

- `ads1299eeg.decode24` — decode three MSB-first ADS1299 bytes into a signed 24-bit code.
- `ads1299eeg.codes_to_uv` — convert signed ADC codes to input-referred microvolts with explicit VREF and PGA gain.

### Preprocessing

- `ads1299eeg.common_average_reference`
- `ads1299eeg.notch_filter`
- `ads1299eeg.bandpass_filter`

The offline filtering functions use zero-phase `filtfilt`. Online/causal BCI filtering should use a dedicated stateful streaming implementation rather than pretending an offline filter is real-time safe.

### Frequency domain

- `ads1299eeg.welch_psd` — self-contained Welch estimator with identical MATLAB/Octave window/overlap semantics.
- `ads1299eeg.bandpower` — absolute and relative delta/theta/alpha/beta/gamma power.
- `ads1299eeg.alpha_peak` — strongest alpha-band PSD frequency.

### Signal quality

- `ads1299eeg.quality_report`

The report includes per-channel:

- mean, standard deviation and RMS;
- peak-to-peak amplitude;
- flatline fraction;
- non-finite fraction;
- clipping fraction;
- line-noise power ratio;
- explicit bad-channel reasons.

Thresholds are engineering/research defaults and should be adapted to the electrodes, montage and experiment.

### Time-domain features

- `ads1299eeg.hjorth` — activity, mobility and complexity.

### Event-related EEG

- `ads1299eeg.epoch_data` — event sample indices to epochs × samples × channels.
- `ads1299eeg.baseline_correct`
- `ads1299eeg.erp_average` — ERP waveform + standard error.

MATLAB event indices are 1-based, matching normal MATLAB indexing.

### Connectivity

- `ads1299eeg.coherence_matrix` — band-limited magnitude-squared coherence.

Connectivity metrics are sensitive to reference choice, volume conduction, filtering and data quality. A numerical matrix is not automatically a physiological conclusion.

### Complete starter pipeline

- `ads1299eeg.basic_pipeline`

Pipeline order:

```text
raw data
  ↓
quality-before
  ↓
remove channel DC mean
  ↓
mains notch
  ↓
0.5-45 Hz (configurable) band-pass
  ↓
optional common-average reference
  ↓
quality-after
  ↓
Welch PSD + band power + alpha peak + Hjorth
```

This is a transparent baseline, not a universal preprocessing prescription.

## Directory structure

```text
eeg_analysis/matlab/
├── +ads1299eeg/
│   ├── validate_continuous.m
│   ├── decode24.m
│   ├── codes_to_uv.m
│   ├── common_average_reference.m
│   ├── notch_filter.m
│   ├── bandpass_filter.m
│   ├── welch_psd.m
│   ├── bandpower.m
│   ├── alpha_peak.m
│   ├── hjorth.m
│   ├── quality_report.m
│   ├── epoch_data.m
│   ├── baseline_correct.m
│   ├── erp_average.m
│   ├── coherence_matrix.m
│   └── basic_pipeline.m
├── examples/
├── tests/
├── eeg_basic_analysis.m          # compatibility wrapper / beginner entry
├── p300_erp_baseline.m           # older educational example
└── ssvep_cca_baseline.m          # older educational example
```

## Validation

Run the numerical suite from the repository root.

MATLAB:

```matlab
addpath('eeg_analysis/matlab');
run('eeg_analysis/matlab/tests/run_tests.m');
```

GNU Octave:

```bash
octave --quiet --eval "pkg load signal; addpath('eeg_analysis/matlab'); run('eeg_analysis/matlab/tests/run_tests.m')"
```

Tests currently verify:

- signed 24-bit ADS1299 decoding;
- code-to-microvolt scale;
- 50-Hz notch attenuation;
- 10-Hz alpha peak recovery;
- expected alpha-band dominance in synthetic EEG;
- flatline bad-channel detection;
- epoch extraction and baseline correction;
- ERP dimensions;
- coherence symmetry/range;
- complete pipeline execution.

## Interoperability instead of disguised copying

ADS1299-Core-SDK can study published algorithms and mature ecosystems such as EEGLAB and FieldTrip, and it can provide import/export or usage examples around them. If third-party source code is ever directly reused, its license, copyright notice and citation requirements must be preserved. Project-native implementations should be written from the underlying method/specification and tested independently rather than line-for-line disguising another repository's code.
