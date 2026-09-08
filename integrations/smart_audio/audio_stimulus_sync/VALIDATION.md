# Audio stimulus synchronization validation checklist

## Software event logging

- [ ] every stimulus has a unique experiment-local event index/ID;
- [ ] host monotonic timestamp is recorded;
- [ ] human-readable wall-clock metadata is kept separately;
- [ ] audio subsystem timing is stored in a separate field when available;
- [ ] EEG sequence/timestamp is never overwritten by host timing;
- [ ] `python test_event_marker.py` passes.

## Physical onset validation

For timing-critical auditory ERP/BCI work:

- [ ] physical audio onset is measured with an independent sensor/path appropriate to the setup;
- [ ] the physical-onset sensor is synchronized to, or has a documented mapping to, the EEG timebase;
- [ ] onset latency distribution is measured over repeated trials;
- [ ] trial-to-trial jitter is reported separately from mean latency;
- [ ] software dispatch time is not presented as measured acoustic onset.

## Clock mapping

- [ ] EEG device clock and host clock are treated as separate time domains;
- [ ] mapping method is documented;
- [ ] offset and drift/scale are estimated over the intended recording duration when needed;
- [ ] device/process restart creates a new mapping segment;
- [ ] residual mapping error is measured rather than assumed zero.

## Event integrity

- [ ] missing event IDs are detectable;
- [ ] duplicate event IDs are detected or explicitly allowed;
- [ ] stimulus ID/condition metadata is reproducible;
- [ ] event JSONL remains parseable after long sessions;
- [ ] event logging cannot grow memory without bound;
- [ ] file flush/storage strategy is tested for unexpected shutdown where important.

## Audio + EEG artifact testing

- [ ] compare EEG with audio idle vs playback active;
- [ ] test high/low output levels within the intended safe research range;
- [ ] test cable/mechanical vibration effects;
- [ ] test wireless audio/network activity if used;
- [ ] record whether playback creates repeatable electrical/mechanical artifact.

## Multi-device / 64-channel

- [ ] eight ADS1299 devices are synchronized before audio alignment is evaluated;
- [ ] one audio event maps to one documented EEG epoch/time estimate;
- [ ] per-device timing diagnostics remain available;
- [ ] inter-device synchronization errors are not hidden by host event alignment.

## Qualification record

Record exact:

- audio endpoint/device;
- audio API/application path;
- host OS/hardware;
- ADS1299 hardware/firmware revision;
- sample rate/channel count;
- stimulus format;
- number of validation trials;
- mean latency, jitter and measurement method;
- clock-mapping method;
- physical-onset measurement setup where used.

Passing this checklist validates experiment timing integration only. It does not establish hearing safety, electrical isolation, EMC or medical-device compliance.
