# OSC validation checklist

## Raw blob mode

- [ ] OSC address includes stable system/device identity;
- [ ] payload is one blob of exactly 49 bytes;
- [ ] canonical sync/version/CRC is validated;
- [ ] sequence/timestamp/status/channels decode correctly;
- [ ] sequence gaps are tracked independently for each device;
- [ ] malformed blob length is rejected.

## Derived feature mode

For every feature stream, record:

- [ ] source channel(s);
- [ ] preprocessing/filtering;
- [ ] processing window length;
- [ ] update rate;
- [ ] units/range;
- [ ] algorithm/version;
- [ ] timestamp/timebase;
- [ ] whether the stream is suitable only for visualization/control or for quantitative analysis.

## Loss/reorder behavior

- [ ] chosen OSC transport is documented;
- [ ] datagram loss/reorder is injected if using an unreliable transport;
- [ ] acquisition-side sequence gaps remain observable;
- [ ] reconnect/restart discontinuities are visible;
- [ ] duplicated messages are not silently counted as new EEG samples.

## 64-channel systems

- [ ] eight source/device addresses map deterministically to device indices 0..7;
- [ ] global channel mapping remains stable;
- [ ] multi-device features use synchronized epochs;
- [ ] missing device packets are not replaced by later samples;
- [ ] sustained message rate is measured at intended sample rate if raw blobs are transmitted.

## Qualification record

Record:

- OSC library/application versions;
- sender and receiver OS/hardware;
- underlying network transport;
- sample/feature update rate;
- ADS1299 device count;
- test duration;
- observed drops/reorder/duplicates;
- CPU/memory/network usage where relevant.

Passing this checklist validates OSC interoperability only. It does not validate ADS1299 analog quality, electrical isolation, EMC or clinical/medical safety.
