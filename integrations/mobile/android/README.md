# Android Reference Protocol Layer

This directory is the first native-mobile protocol implementation for ADS1299-Core on Android/Kotlin.

## Included

- `Ads1299Packet.kt` — 49-byte canonical packet parser, CRC16-CCITT-FALSE, signed channels and physical-unit helper.
- `Ads1299StreamParser.kt` — arbitrary-chunk stream parser and sequence tracking.
- `Ads1299ProtocolSmokeTest.kt` — protocol smoke test without Android UI dependencies.
- `DIRECT_USE.md` — how to add the code to an Android project.
- `TRANSPORTS.md` — BLE/USB/Wi-Fi/WebSocket/UART boundaries.
- `VALIDATION.md` — platform/device acceptance checklist.

## Design rule

Keep transport, protocol, recording and UI as separate layers:

`BLE/USB/network -> transport framing -> canonical packet validation -> recorder/model -> UI`

This makes the 49-byte protocol reusable across phones/tablets and prevents Android lifecycle/UI behavior from changing raw acquisition semantics.

## Current scope

Implemented in this folder:

- pure Kotlin canonical decoder;
- stream resynchronization;
- CRC validation;
- sequence-gap accounting;
- direct integration guidance.

Not claimed as hardware-validated here:

- a specific Android BLE GATT implementation;
- a specific USB VID/PID/driver accessory;
- background acquisition across every Android vendor/OS;
- 64-channel sustained performance on a particular phone.

Those claims require testing on the actual application, phone/tablet and ADS1299 hardware.
