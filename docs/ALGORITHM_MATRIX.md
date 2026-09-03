# EEG / BCI algorithm matrix

Status values: `Included`, `Planned`, `External integration`, `Dataset-tested`, `Hardware-tested`.

| Method | Python | MATLAB | R | Category |
|---|---|---|---|---|
| DC removal / detrend | Included | Included | Included-basic | preprocessing |
| CAR / rereference | Included | Included | Included-basic | preprocessing |
| notch / band-pass | Included | Included | Planned-package | preprocessing |
| Welch PSD | Included | Included | Included-basic FFT | spectral |
| band power | Included | Included | Included-basic | spectral |
| alpha peak | Included | Included | Included-basic | spectral |
| Hjorth parameters | Included | Included | Planned | features |
| ERP / baseline | Included | Included | Included | ERP |
| coherence | Included | Planned | Planned | connectivity |
| ICA | External integration | External integration | Planned | artifact |
| ASR | Planned/integration | External integration | Planned | artifact |
| STFT / wavelets | Planned | Planned | Planned | time-frequency |
| ERD/ERS | Planned | Planned | Planned | BCI/MI |
| CSP | Included | Planned | Planned | BCI/MI |
| FBCSP | Planned | Planned | Planned | BCI/MI |
| CCA SSVEP | Included | Included | Included | BCI/SSVEP |
| FBCCA | Planned | Planned | Planned | BCI/SSVEP |
| xDAWN | External integration | Planned | Planned | BCI/P300 |
| LDA / SVM | Planned | Planned | Planned | ML |
| Riemannian covariance | External integration | Planned | Planned | ML/BCI |
| PLV / PLI / wPLI | Planned | Planned | Planned | connectivity |
| PAC | Planned | Planned | Planned | coupling |
| microstates | Planned/integration | Planned | Planned | EEG states |
| sleep features/staging | Planned | Planned | Planned | sleep |
| EEGNet | Planned | Planned | n/a | deep learning |

The table records repository coverage, not scientific superiority. Mature ecosystems such as MNE-Python, EEGLAB, FieldTrip, MOABB and pyRiemann should be used directly where appropriate.
