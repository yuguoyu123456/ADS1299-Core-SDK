# Ear-EEG acquisition validation checklist

## Electrical / acquisition bring-up

- [ ] ADS1299 ID and internal-test path pass before attaching electrodes;
- [ ] input-short noise is characterized before ear-worn testing;
- [ ] reference and BIAS roles are documented;
- [ ] channel mapping matches the physical electrode harness;
- [ ] no undocumented analog input or shield path is tied to a consumer-audio ground.

## Montage and fit

- [ ] every channel has explicit electrode/montage metadata;
- [ ] left/right/location labels match the actual hardware placement;
- [ ] reseat/fit changes are logged during experiments;
- [ ] motion, jaw movement and facial-muscle artifact are deliberately tested;
- [ ] audio playback state is included in artifact testing if a transducer is physically near the electrodes.

## Transport

- [ ] canonical 49-byte packets validate correctly;
- [ ] sequence gaps are tracked;
- [ ] wireless/USB transport does not block DRDY handling;
- [ ] queues are bounded;
- [ ] reconnect behavior is defined and tested;
- [ ] intended sample rate/channel count is sustained in a soak test.

## RF / digital-noise interaction

Compare input-short and representative electrode recordings with:

- [ ] radio idle;
- [ ] continuous BLE/Wi-Fi traffic;
- [ ] local storage writes;
- [ ] display/processor activity if present;
- [ ] audio playback active/inactive.

Document any repeatable interference before declaring the architecture suitable.

## Low-power mode

- [ ] each operating mode has explicit acquisition/radio/storage behavior;
- [ ] average/peak current is measured rather than guessed;
- [ ] queue high-water/overflow counters remain visible;
- [ ] battery-life claims record the exact test conditions;
- [ ] low-power transitions do not corrupt or silently truncate recordings.

## 64-channel / multi-device ear research

If multiple ADS1299 devices are used:

- [ ] device indices are stable;
- [ ] global channel mapping is deterministic;
- [ ] synchronization is validated independently of packet arrival order;
- [ ] BIAS ownership is explicit;
- [ ] one missing device does not shift later channel identities.

## Safety boundary

Passing this checklist does not establish:

- biocompatibility;
- hearing safety;
- subject leakage-current compliance;
- electrical isolation certification;
- EMC certification;
- clinical/medical-device compliance.

Those require separate hardware/safety/regulatory validation.
