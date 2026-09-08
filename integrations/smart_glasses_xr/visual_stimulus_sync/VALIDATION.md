# XR visual stimulus validation checklist

## Event logging

- [ ] `python test_visual_event_marker.py` passes;
- [ ] each stimulus has a unique event index/ID;
- [ ] condition/stimulus identity is explicit;
- [ ] host monotonic time is recorded;
- [ ] intended frame index is recorded when frame-scheduled;
- [ ] presentation timestamp is recorded only if the platform actually exposes one;
- [ ] EEG sequence/timestamp is preserved separately.

## SSVEP

- [ ] intended visual pattern/frame sequence is documented;
- [ ] actual display refresh behavior is characterized on target hardware;
- [ ] dropped/repeated frame behavior is measured or monitored where possible;
- [ ] effective stimulus timing is verified rather than inferred only from nominal refresh rate;
- [ ] transport/render workload does not destabilize the stimulus schedule.

## P300 / ERP

- [ ] target/non-target/stimulus class is logged per event;
- [ ] event timing reference used for epoching is documented;
- [ ] software dispatch, presentation and physical onset are not conflated;
- [ ] repeated trials characterize onset latency/jitter;
- [ ] event sequence gaps/duplicates are detectable.

## Physical timing validation

For timing-critical experiments:

- [ ] optical onset is measured with a synchronized photodetector/reference path appropriate to the display;
- [ ] latency distribution/jitter is measured across repeated events;
- [ ] measurement method and sensor placement are documented;
- [ ] platform/OS/display mode is recorded.

## EEG synchronization

- [ ] ADS1299 sequence gaps are tracked;
- [ ] device timestamp remains source timing;
- [ ] host-to-EEG clock mapping is documented when used;
- [ ] 8×ADS1299 synchronization is validated before visual-event alignment in 64-channel studies.

## Qualification record

Record:

- XR/glasses device model;
- OS/runtime version;
- application/render API path;
- display mode/refresh behavior;
- ADS1299 hardware/firmware revision;
- sample rate/channel count;
- paradigm/stimulus settings;
- timing measurement method;
- latency/jitter results;
- test duration/frame-drop statistics.

Passing this checklist validates visual stimulus timing integration only. It does not establish clinical validity, accessibility efficacy, participant safety, electrical safety or medical-device compliance.
