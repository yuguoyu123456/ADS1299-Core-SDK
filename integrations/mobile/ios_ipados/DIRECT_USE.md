# iOS / iPadOS Direct Use

This directory contains a pure Swift protocol layer that can be added to an iOS/iPadOS project without coupling packet decoding to SwiftUI, UIKit, CoreBluetooth or Network.framework.

## Files

- `Ads1299Packet.swift` — canonical 49-byte packet decoder, CRC and counts-to-microvolts helper.
- `Ads1299StreamParser.swift` — arbitrary stream chunk recovery and sequence tracking.
- `Ads1299ProtocolSmokeTest.swift` — protocol smoke test.
- `TRANSPORTS.md` — CoreBluetooth/network/wired integration boundary.
- `VALIDATION.md` — platform/device acceptance checklist.

## 1. Add the protocol sources

Add `Ads1299Packet.swift` and `Ads1299StreamParser.swift` to the application target or a reusable Swift package.

The protocol files depend only on Foundation.

## 2. Feed canonical stream bytes

Keep one parser instance for the lifetime of a logical stream:

```swift
let parser = Ads1299StreamParser()
let sequence = Ads1299SequenceTracker()

func onBytes(_ data: Data) {
    for packet in parser.feed(data) {
        sequence.observe(packet.sequence)
        // queue raw packet/counts to recorder/model
    }
}
```

Do not assume one network/USB read equals one 49-byte packet.

## 3. CRC boundary

`Ads1299Protocol.decode()` requires:

- exactly 49 bytes;
- sync `A5 5A`;
- protocol version 0;
- CRC16-CCITT-FALSE over bytes 2..46.

Only validated packets should reach the authoritative recorder.

## 4. Counts vs microvolts

Raw counts should remain primary. For display/analysis conversion:

```swift
let uv = Ads1299Protocol.codeToMicrovolts(
    packet.channels[0],
    vrefVolts: actualVref,
    gain: actualGain
)
```

Use Vref/PGA metadata from the real acquisition configuration.

## 5. Concurrency

Recommended layering:

- transport callback/task receives bytes/fragments;
- protocol actor/serial queue parses and validates;
- recorder persists raw canonical packets/counts;
- UI observes a bounded/downsampled model.

The provided parser is a mutable class; serialize access to one parser instance. `@unchecked Sendable` is not a claim that simultaneous unsynchronized calls are safe.

## 6. 64-channel use

One canonical packet is one ADS1299/8 channels. For 64 channels preserve:

- device/slot identity;
- per-device packet sequence;
- timestamp relation;
- global 1..64 mapping;
- complete/incomplete sample-group state.

Do not map channels by callback arrival order.

## 7. Software smoke test

The smoke test exercises:

- signed channel decode;
- CRC validation;
- arbitrary chunks;
- corruption recovery;
- sequence gaps;
- uint32 wrap-around.

It can be adapted into XCTest/Swift Package tests. Passing it does not prove CoreBluetooth or a specific Apple device/ADS1299 board has been hardware-tested.
