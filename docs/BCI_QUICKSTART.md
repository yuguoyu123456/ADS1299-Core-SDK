# BCI quick start

The BCI folder starts **after** acquisition quality has been established. Do not use a classifier to hide bad electrodes, saturation, mains interference or packet loss.

## SSVEP

Recommended learning path:

1. inspect PSD and harmonics
2. run `bci/ssvep/cca_baseline.py`
3. compare `fbcca_baseline.py`
4. validate on a named public dataset
5. only then connect to the online ADS1299 stream

## Motor imagery

1. epoch trials around cue events
2. inspect mu/beta ERD/ERS
3. extract CSP log-variance features
4. train/test a simple LDA baseline with subject-wise or session-wise leakage controls
5. compare Riemannian covariance methods

## P300

1. epoch and baseline-correct
2. compare target vs non-target ERP
3. inspect P300 amplitude/latency
4. add xDAWN + LDA only after event timing is verified

## Scientific hygiene

- separate training and test data before tuning
- report channel montage, reference, sample rate, filters and epoch windows
- avoid filtering across trial boundaries unless justified
- record event timing latency/jitter
- report class balance and chance level
- prefer established packages for publication pipelines; the local code is for understanding and reproducible baselines
