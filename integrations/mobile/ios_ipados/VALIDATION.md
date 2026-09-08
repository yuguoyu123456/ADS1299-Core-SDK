# iOS / iPadOS Integration Validation

Use this checklist before calling a specific Apple-device/transport combination supported.

## A. Protocol layer

- [ ] Swift smoke test passes.
- [ ] canonical decode matches C implementation.
- [ ] CRC corruption is rejected.
- [ ] arbitrary receive chunking is recovered.
- [ ] signed channels remain correct.
- [ ] sequence gaps are counted.
- [ ] uint32 sequence wrap-around is handled.

## B. Device matrix

Record for every tested target:

- [ ] iPhone/iPad model;
- [ ] iOS/iPadOS version;
- [ ] app commit;
- [ ] firmware commit;
- [ ] transport;
- [ ] ADS1299 device count/sample rate.

Do not label untested Apple devices/OS versions as validated.

## C. CoreBluetooth

- [ ] discovery/filtering finds the intended peripheral;
- [ ] connect/disconnect/reconnect behavior is correct;
- [ ] service/characteristic discovery matches firmware;
- [ ] repository BLE fragments are reassembled correctly;
- [ ] fragment loss/order errors are visible;
- [ ] canonical CRC is validated after reassembly;
- [ ] sustained target throughput is measured;
- [ ] background/screen-lock behavior is measured when required;
- [ ] notification callbacks are not blocked by UI/file work.

## D. Network

- [ ] raw canonical TCP vs WebSocket JSON mode is explicit;
- [ ] network reconnect creates a session boundary;
- [ ] arbitrary TCP chunking is parsed correctly;
- [ ] Ethernet/Wi-Fi block framing is decoded before canonical parsing when used;
- [ ] Wi-Fi loss/roaming behavior is recorded;
- [ ] full target throughput is sustained.

## E. Recording

- [ ] validated raw canonical packets/counts are preserved;
- [ ] Vref/gain/sample-rate metadata matches the actual hardware configuration;
- [ ] write queue is bounded;
- [ ] overflow/loss counters are visible;
- [ ] normal stop flushes/closes files;
- [ ] disconnect/interruption leaves an explicit gap/session marker;
- [ ] storage capacity is sufficient for target session length.

## F. UI

- [ ] UI displays the correct channel order;
- [ ] rendering uses bounded history;
- [ ] display rate can be decoupled from acquisition rate;
- [ ] UI lag cannot silently alter raw recording;
- [ ] sequence/reconnect/integrity state is visible.

## G. 64 channels

- [ ] eight device/slot identities are deterministic;
- [ ] global channel map 1..64 is correct;
- [ ] sample grouping/timestamp relation is defined;
- [ ] sustained transport/recording is benchmarked;
- [ ] CPU/memory/thermal behavior is observed;
- [ ] long-duration capture has no unexplained loss.

## H. Lifecycle/background

If acquisition is expected to continue while backgrounded/locked:

- [ ] the exact transport/use case is compatible with the selected app architecture;
- [ ] behavior is tested on the target device/OS;
- [ ] OS suspension/interruption is detectable;
- [ ] data continuity is never fabricated across suspension;
- [ ] reconnect/resume behavior is documented.

## I. Release evidence

Store:

- test duration;
- packets/messages received;
- CRC/parser failures;
- sequence gaps/missing count;
- reconnects;
- queue overflows;
- device/OS/transport;
- file integrity;
- known limitations.

The Swift protocol smoke test alone does not establish CoreBluetooth/network/wired hardware validation.
