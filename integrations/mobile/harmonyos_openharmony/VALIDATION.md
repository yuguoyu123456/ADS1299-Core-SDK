# HarmonyOS / OpenHarmony Integration Validation

Use this checklist before claiming a particular HarmonyOS/OpenHarmony device and transport are supported.

## A. Protocol layer

- [ ] `Ads1299ProtocolSmokeTest.ts` is compiled/run in the actual project toolchain.
- [ ] 49-byte packet decode matches the repository C implementation.
- [ ] CRC corruption is rejected.
- [ ] arbitrary receive chunking is recovered.
- [ ] signed channel values remain correct.
- [ ] sequence gaps are counted.
- [ ] uint32 sequence wrap-around is handled.

## B. Device/platform matrix

For every tested target record:

- [ ] device model;
- [ ] HarmonyOS/OpenHarmony version;
- [ ] application/SDK/toolchain version;
- [ ] app commit;
- [ ] firmware commit;
- [ ] transport;
- [ ] ADS1299 device count/sample rate.

Do not generalize one platform test to every HarmonyOS/OpenHarmony device.

## C. BLE

- [ ] discovery/permission flow works on the target OS/SDK;
- [ ] connection and reconnect are reliable;
- [ ] repository BLE fragments reassemble correctly;
- [ ] missing/out-of-order fragments are visible;
- [ ] canonical CRC is validated after reassembly;
- [ ] transport callback is not blocked by rendering/file I/O;
- [ ] target sustained throughput is measured;
- [ ] foreground/background/screen-lock behavior is recorded when relevant.

## D. Network / WebSocket

- [ ] raw canonical TCP vs Ethernet block vs WebSocket JSON mode is explicit;
- [ ] reconnect creates a session boundary;
- [ ] arbitrary TCP chunking is handled;
- [ ] gateway JSON is schema-validated when used;
- [ ] Wi-Fi disconnect/roam behavior is recorded;
- [ ] target throughput is sustained.

## E. USB / serial bridge

When used on a supported device:

- [ ] attach/open/permission path is validated;
- [ ] arbitrary USB reads are handled as a byte stream;
- [ ] repository UART COBS framing is decoded before canonical validation when applicable;
- [ ] disconnect is explicit;
- [ ] long-duration capture has no unexplained loss.

## F. Recording

- [ ] validated raw canonical packets/counts are preserved;
- [ ] sample-rate/gain/Vref metadata matches the real hardware configuration;
- [ ] queues are bounded;
- [ ] queue high-water mark is measurable;
- [ ] overflow/drop counters are visible;
- [ ] normal stop/disconnect flushes and closes recording safely;
- [ ] storage capacity is sufficient for the intended session.

## G. UI

- [ ] channel mapping is correct;
- [ ] display history is bounded;
- [ ] display update rate can differ from acquisition rate;
- [ ] UI lag cannot silently alter raw recording;
- [ ] sequence/reconnect/integrity warnings are visible.

## H. 64 channels

- [ ] eight device/slot identities are deterministic;
- [ ] global channels 1..64 map correctly;
- [ ] per-device sequence and timestamps are preserved;
- [ ] complete/incomplete sample grouping is explicit;
- [ ] sustained transport bandwidth is measured;
- [ ] CPU/memory/thermal behavior is observed;
- [ ] long-duration raw recording has no unexplained loss.

## I. Lifecycle/background

If recording should continue outside the foreground:

- [ ] behavior is tested on the exact device/OS/app model;
- [ ] suspension/restart is detected;
- [ ] interruption is represented in metadata;
- [ ] no synthetic samples are inserted across an OS suspension gap.

## J. Release evidence

Store:

- test duration;
- packets/messages received;
- parser/CRC failures;
- missing packet count;
- reconnects;
- queue overflows;
- device/OS/transport;
- file integrity;
- known limitations.

Source code and a portable protocol test do not by themselves prove real HarmonyOS/OpenHarmony BLE/USB/network hardware compatibility.
