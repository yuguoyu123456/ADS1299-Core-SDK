# iOS / iPadOS Reference Protocol Layer

This directory provides the native Apple-platform protocol foundation for ADS1299-Core.

## Included

- `Ads1299Packet.swift` — canonical 49-byte packet parser, CRC and counts-to-microvolts helper.
- `Ads1299StreamParser.swift` — arbitrary byte-stream recovery and sequence tracking.
- `Ads1299ProtocolSmokeTest.swift` — protocol smoke test.
- `DIRECT_USE.md` — integration instructions.
- `TRANSPORTS.md` — CoreBluetooth/network/wired transport boundaries.
- `VALIDATION.md` — Apple-device validation checklist.

## Architecture

Keep the layers separate:

`CoreBluetooth/network/accessory -> transport framing -> canonical validation -> recorder/model -> SwiftUI/UIKit`

This prevents UI/lifecycle code from redefining the raw acquisition protocol.

## Implemented here

- pure Swift/Foundation canonical decoder;
- CRC16-CCITT-FALSE;
- signed eight-channel decode;
- arbitrary stream chunk recovery;
- sequence-gap accounting;
- direct-use and transport guidance.

## Not claimed here

This directory does not claim hardware validation for a specific:

- iPhone/iPad;
- CoreBluetooth peripheral/service definition;
- wired USB/accessory implementation;
- background-execution configuration;
- 64-channel sustained mobile recording setup.

Those claims require testing on the actual Apple device, app, transport and ADS1299 hardware.
