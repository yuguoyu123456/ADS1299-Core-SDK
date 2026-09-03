# BCI Algorithms

This folder contains educational and research reference implementations that sit **above** the ADS1299 acquisition layer.

Planned tracks:

- `motor_imagery/`: ERD/ERS, CSP, FBCSP, LDA/SVM, Riemannian baselines
- `ssvep/`: FFT/PSD baseline, CCA, FBCCA, harmonic SNR
- `p300/`: ERP averaging, xDAWN, LDA/SWLDA baselines
- `online/`: ring buffers, windowing, streaming inference, latency and packet-loss checks
- `ear_eeg/`: compact-channel analysis examples
- `sleep_bci/`: sleep-oriented EEG feature examples

## Validation labels

Every BCI example must state one of:

- `Educational`: demonstrates the method only
- `Dataset-tested`: verified on a named public dataset
- `Hardware-tested`: verified with ADS1299-Core hardware
- `Online-tested`: verified in a real-time loop

No example should be presented as clinically validated or medical-device software.
