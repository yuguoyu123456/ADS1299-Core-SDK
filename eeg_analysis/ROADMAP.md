# EEG / BCI Algorithm Roadmap

This roadmap organizes analysis examples by maturity and practical value. The goal is not to reimplement every scientific toolbox, but to provide clear starter implementations plus integrations with mature ecosystems.

## A. Beginner / foundational

- [x] DC removal / detrending
- [x] common-average reference (CAR)
- [x] single-reference rereferencing
- [x] notch filter
- [x] band-pass filter
- [x] Welch PSD
- [x] absolute and relative band power
- [x] alpha peak
- [x] RMS / peak-to-peak / standard deviation
- [x] Hjorth parameters
- [x] epoching
- [x] baseline correction
- [x] amplitude-threshold artifact rejection
- [x] ERP averaging
- [x] magnitude-squared coherence

## B. Time-frequency and rhythms

- [ ] STFT spectrogram
- [ ] Morlet wavelet transform
- [ ] event-related spectral perturbation (ERSP)
- [ ] ERD / ERS
- [ ] individual alpha frequency (IAF) robust estimator
- [ ] spectral edge frequency
- [ ] spectral entropy

## C. Artifact and signal quality

- [ ] bad-channel scoring
- [ ] flatline / clipping / saturation detection
- [ ] line-noise ratio
- [ ] robust z-score artifact flags
- [ ] ICA example using a mature toolbox
- [ ] EOG regression example
- [ ] ASR integration example

## D. ERP / cognitive EEG

- [ ] P100 / N100 / N200 / P300 measurement helpers
- [ ] peak amplitude and latency
- [ ] mean-window amplitude
- [ ] global field power
- [ ] xDAWN spatial filters
- [ ] ERP classification baseline

## E. Motor imagery / sensorimotor BCI

- [ ] band-power ERD/ERS pipeline
- [ ] CSP
- [ ] FBCSP
- [ ] LDA baseline
- [ ] SVM baseline
- [ ] Riemannian covariance classifier

## F. SSVEP

- [ ] frequency-template baseline
- [ ] CCA
- [ ] FBCCA
- [ ] harmonic SNR
- [ ] reference-signal generation

## G. Connectivity

- [x] magnitude-squared coherence
- [ ] Pearson / Spearman correlation
- [ ] PLV
- [ ] PLI
- [ ] wPLI
- [ ] imaginary coherence
- [ ] graph-metric starter examples

## H. Complexity / nonlinear features

- [ ] sample entropy
- [ ] permutation entropy
- [ ] Lempel-Ziv complexity
- [ ] fractal dimension
- [ ] DFA

## I. Sleep / long-duration EEG

- [ ] delta/theta/alpha/beta summary per epoch
- [ ] spindle candidate detection
- [ ] slow-wave candidate detection
- [ ] simple sleep-feature extraction
- [ ] interface example for external validated sleep-staging models

## J. Machine learning

- [ ] train/validation/test split by subject
- [ ] feature standardization without leakage
- [ ] logistic regression
- [ ] LDA
- [ ] SVM
- [ ] random forest
- [ ] cross-validation templates
- [ ] confusion matrix / ROC / PR metrics
- [ ] subject-independent evaluation example

## K. Mature toolbox integrations

### Python
- [ ] MNE-Python Raw/Epochs conversion
- [ ] MNE filtering/ICA/PSD example
- [ ] scikit-learn pipeline
- [ ] pyRiemann example
- [ ] MOABB-compatible BCI benchmark example

### MATLAB
- [ ] EEGLAB import example
- [ ] FieldTrip import example
- [ ] EEGLAB ICA workflow notes
- [ ] FieldTrip time-frequency workflow notes

### R
- [ ] tidy EEG data frame example
- [ ] `signal` package filtering example
- [ ] ggplot2 visualization example
- [ ] mixed-effects/statistical workflow notes

## Validation philosophy

Every algorithm example should state:

1. expected input shape and units
2. sampling-rate requirements
3. assumptions
4. default parameters
5. references/toolbox equivalents
6. whether output was tested on synthetic, public or real ADS1299 data

A checkbox means the code exists in the repository; it does **not** mean a method is clinically validated.
