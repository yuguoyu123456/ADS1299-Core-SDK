# HarmonyOS / OpenHarmony Direct Use

This directory keeps the ADS1299-Core protocol layer independent from a specific HarmonyOS/OpenHarmony UI or Bluetooth API release.

## Included core files

- `Ads1299Protocol.ts` — canonical 49-byte packet decoder, CRC16-CCITT-FALSE and counts-to-microvolts helper.
- `Ads1299StreamParser.ts` — arbitrary byte-stream recovery and sequence tracking.
- `Ads1299ProtocolSmokeTest.ts` — protocol-level smoke test source.

The protocol code uses standard TypeScript-style `Uint8Array`/number operations so it can be adapted into an ArkTS/OpenHarmony application layer without changing the wire format.

## Canonical packet boundary

Every ADS1299 device sample in the repository canonical format is exactly 49 bytes:

- `A5 5A` sync;
- version and flags;
- uint32 sequence;
- uint32 `timestamp_us`;
- 3 status bytes;
- 8 signed int32 channel values;
- CRC16.

Always validate this canonical layer after any BLE/UART/network transport framing is removed.

## Raw byte stream use

For a USB/network byte stream that carries raw canonical packets, keep one parser instance:

```ts
const parser = new Ads1299StreamParser();
const sequence = new Ads1299SequenceTracker();

function onBytes(chunk: Uint8Array): void {
  for (const packet of parser.feed(chunk)) {
    sequence.observe(packet.sequence);
    // Queue packet.raw / packet.channels for recording or UI.
  }
}
```

Do not assume one socket/read callback equals one packet.

## Keep acquisition and UI separated

Recommended application architecture:

`BLE/network/USB callback -> transport decoder -> canonical parser -> bounded recorder/model queue -> UI`

Do not do expensive plotting, database operations or file export in a high-rate transport callback.

## Counts before physical units

Preserve raw signed counts in the authoritative recording. Convert to microvolts only with the actual hardware metadata:

```ts
const uv = Ads1299Protocol.codeToMicrovolts(code, actualVref, actualGain);
```

Do not embed assumed Vref/gain values into the protocol parser.

## 64-channel use

Eight ADS1299 devices remain eight canonical packet sources. Preserve:

- device/slot index;
- each device sequence;
- timestamp relation;
- global channel map 1..64;
- complete/incomplete sample-group status.

Do not concatenate device channels based only on callback arrival order.

## Smoke test

`Ads1299ProtocolSmokeTest.ts` covers:

- packet decode;
- CRC validation;
- signed channel values;
- arbitrary chunk boundaries;
- recovery after a corrupted packet;
- missing-sequence accounting;
- uint32 wrap-around.

Run/adapt it with the TypeScript/ArkTS toolchain used by the actual application. This repository does not claim that the test has been executed on a specific HarmonyOS/OpenHarmony SDK or device merely because the source exists.
