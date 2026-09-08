# Audio neurofeedback validation checklist

## Controller

- [ ] `python test_feedback_controller.py` passes;
- [ ] input score meaning/range is documented;
- [ ] output remains bounded to the documented abstract range;
- [ ] invalid/non-finite/missing scores move to a defined neutral/suppressed state;
- [ ] smoothing parameters are recorded;
- [ ] controller configuration is saved with the experiment/session.

## EEG/feature pipeline

- [ ] canonical EEG sequence/timestamps are preserved;
- [ ] preprocessing is reproducible;
- [ ] feature algorithm/version is documented;
- [ ] feature window length/step/overlap are recorded;
- [ ] artifact/signal-validity rule is explicit;
- [ ] raw EEG is retained independently of the feedback score where the experiment protocol requires reproducibility.

## Closed-loop timing

Measure or document:

- [ ] acquisition-to-host delay;
- [ ] feature-window completion delay;
- [ ] preprocessing/feature computation time;
- [ ] controller update time;
- [ ] audio dispatch time;
- [ ] audio/physical onset latency when relevant;
- [ ] end-to-end latency distribution/jitter.

Do not report only one generic latency number if the stages can be separated.

## Audio endpoint

- [ ] audio endpoint adapter uses documented capabilities;
- [ ] software dispatch is not mislabeled as acoustic onset;
- [ ] reconnect/failure behavior is defined;
- [ ] feedback output is mapped to an independently validated safe audio parameter by the endpoint layer;
- [ ] participant comfort/hearing-safety procedures are governed separately from this controller code.

## Failure handling

Test:

- [ ] EEG packet gap;
- [ ] invalid signal-quality interval;
- [ ] feature computation exception/error;
- [ ] audio endpoint disconnect;
- [ ] network congestion;
- [ ] application restart;
- [ ] multi-device missing epoch where applicable.

Every failure mode should produce an explicit state/event rather than silently continuing with stale feedback.

## 64-channel

- [ ] synchronized epoch assembly is validated before multi-channel feature computation;
- [ ] global channel mapping is deterministic;
- [ ] missing device/source behavior is explicit;
- [ ] loop latency remains acceptable at the intended channel count/sample rate;
- [ ] queue/memory growth remains bounded in a soak test.

## Scientific/clinical boundary

Passing this checklist validates the software closed-loop engineering path only. It does not establish therapeutic benefit, diagnosis, treatment efficacy, hearing safety, electrical safety, clinical validity or medical-device compliance.
