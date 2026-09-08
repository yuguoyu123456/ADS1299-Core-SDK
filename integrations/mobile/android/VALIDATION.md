# Android Integration Validation

Use this checklist before claiming a particular Android transport/device combination is supported.

## A. Protocol layer

- [ ] Kotlin smoke test passes.
- [ ] 49-byte packet decode matches the C implementation.
- [ ] CRC corruption is rejected.
- [ ] arbitrary byte chunking is recovered.
- [ ] negative channel values remain signed.
- [ ] sequence gaps are counted.
- [ ] uint32 sequence wrap-around is handled.

## B. Android device matrix

For each tested device record:

- [ ] phone/tablet model;
- [ ] Android version;
- [ ] CPU architecture;
- [ ] transport (BLE/USB/Wi-Fi/etc.);
- [ ] application commit;
- [ ] firmware commit;
- [ ] ADS1299 device count/sample rate.

Do not generalize one phone test to every Android device.

## C. BLE

- [ ] device discovery/permission flow works on the tested Android version;
- [ ] connect/disconnect/reconnect works;
- [ ] negotiated payload behavior is recorded;
- [ ] repository fragment reassembly is correct;
- [ ] malformed/missing/out-of-order fragment behavior is tested;
- [ ] canonical CRC is validated after reassembly;
- [ ] screen lock/background behavior is measured if recording must continue;
- [ ] sustained throughput has margin for target rate/channel count.

## D. USB

- [ ] device attach/detach behavior is handled;
- [ ] permission/open path works;
- [ ] arbitrary USB read sizes feed the stream parser correctly;
- [ ] disconnect produces an explicit session boundary;
- [ ] long-duration capture has no unexplained packet gaps;
- [ ] target Android device can supply/accept the intended USB power/accessory mode safely.

## E. Wi-Fi/TCP/WebSocket

- [ ] network disconnect/reconnect is visible;
- [ ] canonical TCP stream or gateway JSON mode is explicitly selected;
- [ ] packet/message identity is not inferred from arrival order;
- [ ] socket callback does not block on UI/disk work;
- [ ] Wi-Fi roam/temporary loss behavior is recorded;
- [ ] target throughput is sustained.

## F. Recorder

- [ ] raw canonical packet/count representation is preserved;
- [ ] metadata includes Vref/gain/sample rate only when known from the real configuration;
- [ ] queue is bounded;
- [ ] queue high-water mark is measurable;
- [ ] overflow/dropped-packet counter is visible;
- [ ] file close/flush behavior is tested after normal stop and disconnect;
- [ ] enough free storage is checked for long sessions.

## G. UI

- [ ] rendering never changes authoritative raw data;
- [ ] bounded display history is used;
- [ ] display update rate can be lower than sample rate;
- [ ] selected-channel/64-channel views preserve global channel mapping;
- [ ] sequence/integrity problems are visible to the user/operator.

## H. 64 channels

- [ ] eight device identities are deterministic;
- [ ] global channels 1-64 map correctly;
- [ ] sample grouping/timestamps are defined;
- [ ] sustained transport bandwidth is measured;
- [ ] app CPU/memory/thermal behavior is recorded;
- [ ] long-duration raw capture is tested;
- [ ] no unexplained loss occurs under simultaneous rendering and recording.

## I. Lifecycle/background

If acquisition is expected to survive app backgrounding or screen lock:

- [ ] behavior is tested explicitly on each target Android version/device;
- [ ] OS power-management restrictions are handled through the appropriate app architecture;
- [ ] interruptions are represented in the session metadata;
- [ ] the app never pretends samples were continuous across a suspension gap.

## J. Release evidence

Record final:

- total packets;
- CRC/parser failures;
- sequence gaps/missing count;
- reconnects;
- queue overflows;
- test duration;
- file size/integrity;
- device/OS/transport;
- known limitations.

Passing the pure Kotlin test does not prove BLE/USB hardware behavior on Android; the platform/device validation must be performed separately.
