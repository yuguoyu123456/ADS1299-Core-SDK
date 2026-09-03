# BCI roadmap

## Phase 1 — classical baselines
- [x] SSVEP CCA educational baseline
- [x] SSVEP FBCCA educational baseline
- [x] Motor-imagery CSP educational baseline
- [x] Motor-imagery ERD/ERS helper
- [x] P300 ERP educational baseline
- [x] Regularized LDA baseline
- [x] Log-Euclidean covariance / Riemannian-style baseline
- [x] Online ring buffer
- [ ] SSVEP harmonic SNR
- [ ] FBCSP
- [ ] SVM example
- [ ] affine-invariant Riemannian / tangent-space baseline
- [ ] xDAWN + LDA

## Phase 2 — ecosystem interoperability
- [x] MNE-Python Raw conversion starter
- [ ] EEGLAB import example
- [ ] FieldTrip import example
- [ ] BrainFlow adapter investigation
- [ ] BIDS/EEG-BIDS export example
- [ ] MOABB reproducible benchmark example

## Phase 3 — ADS1299 real-time demos
- [x] common online ring buffer
- [x] common packet serial reader
- [x] engineering live viewer
- [ ] 8-channel SSVEP online demo
- [ ] 8-channel motor-imagery online demo
- [ ] 8-channel P300 online demo
- [ ] ear-EEG feature demo
- [ ] sleep EEG feature demo

## Phase 4 — modern ML (after classical baselines are verified)
- [ ] EEGNet reference training pipeline
- [ ] ShallowConvNet reference pipeline
- [ ] reproducible train/validation/test splits
- [ ] ONNX/TFLite deployment notes where practical

## Validation rule

Checked boxes mean source exists in this repository, not that scientific performance has been validated. Dataset, hardware and online validation are tracked separately.
