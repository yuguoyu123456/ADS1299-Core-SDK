# LSL validation checklist

## Bridge input

- [ ] canonical packet sync/version/CRC is validated before LSL conversion;
- [ ] sequence gaps are tracked before sample publication;
- [ ] channel order is stable;
- [ ] raw counts or physical-unit conversion mode is explicit;
- [ ] Vref/PGA settings are recorded if converting to microvolts.

## Stream metadata

- [ ] channel count is correct;
- [ ] sample rate metadata matches acquisition configuration;
- [ ] channel labels/order are documented;
- [ ] units are explicit;
- [ ] system/device identifier is stable;
- [ ] montage/reference metadata is recorded where needed for the experiment.

## Timing

- [ ] source/device timestamp is preserved somewhere in the recording/integration path;
- [ ] host/LSL timestamp strategy is documented;
- [ ] network/USB receive time is not silently substituted for source acquisition time;
- [ ] long-duration clock behavior/drift is measured for the intended setup;
- [ ] event-marker timebase is documented and aligned with EEG.

## Multi-device / 64-channel

- [ ] all eight sources have stable identity;
- [ ] epoch aggregation is completed before publishing one 64-channel sample;
- [ ] global channel order is deterministic;
- [ ] per-device timestamp/status can be inspected for synchronization diagnostics;
- [ ] missing source packets create explicit discontinuities rather than channel shifting;
- [ ] 64-channel sample rate is sustained without host-side queue growth.

## Interoperability testing

Validate with the actual research consumer(s) used in the experiment and record:

- host OS;
- liblsl version/build;
- outlet implementation revision;
- receiver/recording software;
- sample rate/channel count;
- test duration;
- sequence gaps;
- observed timing/jitter/drift metrics;
- CPU/memory usage where relevant.

## Boundary

Passing this checklist validates the host LSL integration. It does not prove ADS1299 analog quality, hardware synchronization, subject isolation, EMC or medical-device compliance.
