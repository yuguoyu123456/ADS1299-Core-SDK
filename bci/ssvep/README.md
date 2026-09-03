# ADS1299 SSVEP BCI Toolkit

`bci/ssvep` is the reusable SSVEP layer of ADS1299-Core-SDK. It is designed for transparent research/education prototypes and for validating an ADS1299 acquisition chain with common SSVEP paradigms.

The package intentionally separates **training-free** methods (spectral scoring, CCA, FBCCA) from **calibration-based** methods (TRCA), and separates offline window classification from the small causal sample-buffer used for online decisions.

## Install

From the repository root:

```bash
python -m pip install -e bci/ssvep
```

Requires Python 3.10+, NumPy and SciPy.

## Data conventions

- EEG window: `samples x channels`
- TRCA calibration set: `trials x samples x channels`
- Sampling rate: Hz
- Signal amplitude: any consistent linear unit (uV recommended for ADS1299 host data)
- Target labels: stimulus frequencies in Hz

## 1. Training-free CCA

```python
from ads1299_ssvep import CCAClassifier, simulate_ssvep_window

fs = 250.0
frequencies = [8, 10, 12, 15]
eeg = simulate_ssvep_window(12, fs, 2.0, seed=7)

classifier = CCAClassifier(frequencies, fs, harmonics=3)
decision = classifier.predict(eeg)

print(decision.label_hz)
print(decision.scores)
print(decision.margin)
```

CCA compares the EEG window with sine/cosine references at each candidate frequency and usable harmonics. The implementation uses regularized covariance whitening and SVD rather than hiding the operation behind a large ML dependency.

## 2. Spectral power and local SNR

```python
from ads1299_ssvep import frequency_snr_db, spectral_scores

snr_12 = frequency_snr_db(eeg, fs, 12, harmonics=3)
scores = spectral_scores(eeg, fs, frequencies, metric="snr_db")
```

The SNR helper compares target/harmonic FFT bins with nearby flanking bins. It is useful for engineering checks and for explaining why a classifier succeeded or failed.

## 3. FBCCA

```python
from ads1299_ssvep import FBCCAClassifier

fbcca = FBCCAClassifier(frequencies, fs, harmonics=3)
decision = fbcca.predict(eeg)
```

FBCCA applies a set of progressively higher band-pass filters, computes CCA in each subband, squares the correlations and combines them with decreasing subband weights. This implementation is project-native and exposes every weighting/filter parameter.

## 4. Calibration-based TRCA

```python
from ads1299_ssvep import TRCAClassifier, simulate_ssvep_dataset

train_x, train_y = simulate_ssvep_dataset(
    frequencies,
    trials_per_class=6,
    fs=fs,
    phase_locked=True,
    seed=10,
)

trca = TRCAClassifier(fs, ridge=1e-4).fit(train_x, train_y)
decision = trca.predict(train_x[0])
```

TRCA needs repeated **stimulus-locked calibration trials**. Do not evaluate it using independently randomized trial phase and then interpret poor accuracy as an implementation defect. Real experiments should align the analysis epoch to a trustworthy visual-stimulus marker and should preserve refresh/display timing metadata.

## 5. Confidence rejection

CCA, FBCCA and TRCA classifier objects support optional `min_score` and `min_margin` thresholds. When a window is weak or ambiguous, the returned `SSVEPDecision` can have `label_hz=None` and `rejected=True` rather than forcing a command.

```python
classifier = CCAClassifier(
    frequencies,
    fs,
    min_score=0.35,
    min_margin=0.05,
)
```

Thresholds must be calibrated on representative validation data; the package does not claim universal clinical/BCI thresholds.

## 6. Streaming window engine

```python
from ads1299_ssvep import SSVEPStreamClassifier

stream = SSVEPStreamClassifier(
    classifier,
    fs,
    window_seconds=2.0,
    hop_seconds=0.25,
)

for block in blocks_from_your_transport:
    for result in stream.push(block):
        print(result.end_time_s, result.decision.label_hz)
```

The stream object provides deterministic ring-buffer/window cadence only. It deliberately does **not** hide transport dropouts, BLE/USB latency, stimulus-marker synchronization or display timing. Those concerns belong in the shared online BCI layer and must be validated separately.

## 7. ITR reporting

```python
from ads1299_ssvep import itr_bits_per_minute

itr = itr_bits_per_minute(
    n_targets=4,
    accuracy=0.90,
    trial_seconds=2.5,
)
```

`trial_seconds` should represent the full decision cycle relevant to the experiment, not merely Python compute time. State your timing assumptions whenever reporting ITR.

## Recommended ADS1299 SSVEP acquisition path

A practical first hardware validation can use:

1. ADS1299 at 250 or 500 SPS.
2. Occipital/parietal electrodes such as O1/Oz/O2 plus suitable reference and BIAS strategy.
3. Visual targets whose actual rendered frequencies are verified against monitor refresh behavior.
4. Raw ADS1299 codes converted to input-referred uV before analysis.
5. A 1.5-3 s initial analysis window.
6. Spectral/SNR inspection first, then CCA/FBCCA.
7. TRCA only after reliable event timing and enough same-condition calibration trials exist.

## Validation included in this repository

The numerical CI checks:

- harmonic-reference construction and Nyquist handling;
- canonical-correlation invariants;
- CCA identification across multiple synthetic target frequencies;
- target-vs-nontarget spectral SNR;
- FBCCA target recovery;
- confidence rejection;
- TRCA calibration and held-out classification on phase-locked synthetic trials;
- online rolling-window cadence;
- ITR and confusion/accuracy metrics;
- compatibility of the historical `cca_baseline.py` and `fbcca_baseline.py` entry points.

Synthetic tests are **algorithm regression tests**, not evidence of real-world BCI accuracy. Real validation still requires an actual display, electrodes, subject data, timing measurements and ADS1299 hardware.

## Source policy

Published algorithms, papers, datasheets and mature open-source BCI projects may be studied to verify expected mathematical behavior and interoperability. Code in this package is implemented as a project-native API. If source code from another project is ever directly incorporated, its original license, notices and attribution requirements must remain intact; source must not be cosmetically rewritten to conceal provenance.
