# BCI Beginner Path

If you are new to brain-computer interfaces, start here before choosing an algorithm.

## Pick a paradigm

- **SSVEP** — look at flickering visual targets and classify the stimulation frequency.
- **Motor imagery** — imagine left/right hand or other movement and classify sensorimotor rhythms.
- **P300** — detect event-related responses to rare or attended stimuli.
- **Online BCI** — connect real-time windows, features, classifiers and feedback.

## Recommended learning order

1. Understand sampling rate, channels, reference and filtering.
2. Confirm clean ADS1299 acquisition first.
3. Run the simplest baseline for one paradigm.
4. Learn train/test separation and cross-validation.
5. Only then move to more advanced methods such as FBCCA, FBCSP, Riemannian geometry or deep learning.

For general EEG preprocessing and signal quality, use `../../eeg_analysis/` first.
