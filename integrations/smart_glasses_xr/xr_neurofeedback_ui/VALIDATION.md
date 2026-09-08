# XR neurofeedback / BCI overlay validation checklist

## State helper

- [ ] `python test_overlay_state.py` passes;
- [ ] EEG sequence/timestamp fields preserve source semantics;
- [ ] all numeric metrics are finite;
- [ ] signal validity is explicit;
- [ ] maximum state age is explicit;
- [ ] stale data produces a neutral/stale/disconnected UI state.

## Render-thread separation

- [ ] raw packet parsing is outside the XR render callback;
- [ ] heavy EEG processing is outside the XR render callback;
- [ ] bounded queues/state snapshots are used between processing and UI;
- [ ] a slow UI cannot block ADS1299 acquisition;
- [ ] raw 64-channel waveform mode is tested separately from participant-facing overlay mode.

## Hands-free BCI commands

- [ ] classifier decision is logged;
- [ ] UI indication is logged where relevant;
- [ ] command intent/dispatch is separate from classifier output;
- [ ] acknowledgement/result is logged;
- [ ] invalid/stale EEG cannot trigger a new command unless explicitly allowed by the experiment protocol;
- [ ] duplicate decisions/commands are handled deliberately.

## Timing

- [ ] overlay receive age is measured using a monotonic clock;
- [ ] EEG source time is kept separately;
- [ ] visual stimulus timing is validated separately for SSVEP/P300 tasks;
- [ ] UI update rate does not destabilize visual stimulus presentation;
- [ ] end-to-end BCI feedback latency/jitter is characterized where scientifically relevant.

## Platform qualification

For each supported glasses/XR target record:

- exact hardware model;
- OS/runtime version;
- SDK/render path;
- transport from ADS1299/edge gateway;
- EEG channel count/sample rate;
- overlay update rate;
- stale timeout;
- soak-test duration;
- dropped UI/network states;
- frame timing/CPU/GPU impact where available.

Do not claim device support from a generic XR architecture document alone.

## Boundary

Passing this checklist validates the research UI integration only. It does not establish diagnostic/therapeutic benefit, accessibility efficacy, participant safety, electrical isolation or medical-device compliance.
