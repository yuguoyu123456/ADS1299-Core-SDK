# BCI — Brain-Computer Interface Workflows

This folder sits above the ADS1299 acquisition and EEG-analysis layers. It is organized by **BCI paradigm first**, with reusable classifiers and mathematical methods separated into `common_methods/`.

## Where should I start?

- New to BCI → [`00_beginner/`](00_beginner/)
- Flickering visual targets → [`ssvep/`](ssvep/)
- Imagined left/right movement → [`motor_imagery/`](motor_imagery/)
- Oddball / P300 experiments → [`p300/`](p300/)
- Real-time buffering and sliding windows → [`online/`](online/)
- Reusable classifiers / Riemannian methods → [`common_methods/`](common_methods/)

## Structure

```text
bci/
├── 00_beginner/              # beginner learning path
├── ssvep/                    # CCA, FBCCA and SSVEP baselines
├── motor_imagery/            # ERD/ERS, CSP and MI workflows
├── p300/                     # ERP/P300 workflows
├── online/                   # ring buffers and online pipeline foundations
└── common_methods/
    ├── classification/       # LDA and reusable classifiers
    └── riemannian/           # covariance / Riemannian baselines
```

## Validation labels

Every BCI example should state the strongest level it has actually reached:

- **Educational** — demonstrates the method and assumptions.
- **Dataset-tested** — verified on a named public dataset.
- **Hardware-tested** — verified using ADS1299-Core acquisition hardware.
- **Online-tested** — verified in a real-time acquisition/classification loop.

A method should not be described as `Hardware-tested` or `Online-tested` until evidence exists.

## Relationship to EEG analysis

General filtering, PSD, signal-quality checks, ERP foundations and feature engineering belong in [`../eeg_analysis/`](../eeg_analysis/). This folder focuses on turning those signals into BCI paradigms, classifiers and real-time workflows.

## Long-term BCI scope

The project roadmap includes:

- SSVEP: FFT/harmonic inspection, CCA, FBCCA and online recognition;
- motor imagery: ERD/ERS, CSP, FBCSP, classical classifiers and Riemannian methods;
- P300: ERP processing, xDAWN-class approaches and classification;
- auditory BCI and other paradigms where reproducible examples are available;
- online latency, dropped-window and packet-loss measurement;
- deep-learning baselines only when they provide a clear, reproducible advantage over classical methods.

These materials are for research, education and engineering development. They are not clinically validated medical-device algorithms.
