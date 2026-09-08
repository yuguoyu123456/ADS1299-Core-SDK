# Closed-loop timing model

Neurofeedback latency is the sum of several independently measurable stages.

## Pipeline stages

```text
EEG acquisition
  -> transport
  -> buffering/window completion
  -> preprocessing
  -> feature computation
  -> feedback controller
  -> audio command dispatch
  -> audio subsystem
  -> physical acoustic output
```

Report these stages separately where possible.

## Window latency

Feature algorithms often require a window of EEG samples. A one-second feature window inherently adds window-completion delay unless the algorithm is explicitly causal/streaming.

Document:

- window length;
- step/update interval;
- overlap;
- filter state/warm-up;
- artifact rejection delay.

## Transport latency

Keep source/device timestamps and host receive timestamps separate so transport jitter can be measured.

Do not infer acquisition time from when a Python/UI callback executed.

## Controller latency

The provided `feedback_controller.py` is intentionally small; controller execution time should normally be minor compared with acquisition windows/audio output, but measure it in the actual application if end-to-end latency matters.

## Audio latency

Audio command dispatch time is not physical onset. Use the timing model and validation approach in:

```text
integrations/smart_audio/audio_stimulus_sync/
```

for experiments that require accurate acoustic-onset measurement.

## Jitter

Report latency distributions, not only mean latency. Useful measures include median, percentile range and worst observed values according to the experiment requirements.

## Invalid signal behavior

When EEG quality is invalid, the controller should enter a documented neutral/suppressed state. Record the time at which signal validity changed so the closed-loop log can distinguish intentional neutral feedback from algorithm output.

## 64-channel systems

For multi-device systems, include epoch assembly time in the loop budget. Do not compute a 64-channel feature until the intended synchronized epoch is complete or the algorithm explicitly supports missing-source behavior.
