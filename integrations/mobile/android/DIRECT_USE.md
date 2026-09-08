# Android Direct Use

This directory contains a protocol layer that can be dropped into an Android/Kotlin project without coupling packet decoding to a specific UI, BLE stack wrapper or device model.

## Files

- `Ads1299Packet.kt` — canonical 49-byte packet decode, CRC and counts-to-microvolts helper.
- `Ads1299StreamParser.kt` — arbitrary-byte stream recovery plus sequence-gap tracking.
- `Ads1299ProtocolSmokeTest.kt` — dependency-light protocol smoke test.

## 1. Add the files

Copy the Kotlin sources into your Android project under a package matching your application, or keep `package ads1299.core` and place them in that package.

No Android framework imports are required by the protocol layer.

## 2. Feed raw canonical bytes

For USB/TCP/WebSocket-binary sources that ultimately deliver the repository canonical 49-byte stream, feed every received byte chunk into one long-lived parser instance:

```kotlin
val parser = Ads1299StreamParser()
val sequence = Ads1299SequenceTracker()

fun onBytes(chunk: ByteArray) {
    for (packet in parser.feed(chunk)) {
        sequence.observe(packet.sequence)
        // hand packet.channels to a bounded recording/display pipeline
    }
}
```

Do not create a new parser for each USB/network read; packet boundaries may span reads.

## 3. CRC boundary

`Ads1299Protocol.decode()` validates:

- 49-byte size;
- sync `A5 5A`;
- protocol version 0;
- CRC16-CCITT-FALSE over bytes 2..46.

Only validated packets should reach the recorder/plotter.

## 4. Raw counts first

Keep raw signed int32 counts as the authoritative data representation. Convert for display only when the real hardware metadata is known:

```kotlin
val uv = Ads1299Protocol.codeToMicrovolts(
    packet.channels[0],
    vrefVolts = actualVref,
    gain = actualGain,
)
```

Do not hard-code a guessed Vref/PGA gain into recorded data.

## 5. Threading

Recommended Android architecture:

- BLE/USB/network callback: copy/queue incoming bytes quickly;
- protocol worker/coroutine: parse/validate packets;
- recorder worker: persist validated raw packets/counts;
- UI: consume a bounded/downsampled view model.

Do not perform expensive chart rendering or file I/O directly in a high-rate transport callback.

## 6. BLE

Repository BLE transport may fragment one canonical packet. Reassemble BLE transport fragments before calling the canonical packet decoder.

The Android application must not treat one BLE notification as one ADS1299 sample unless the negotiated application payload is large enough and the firmware explicitly sends one whole packet per notification.

See `TRANSPORTS.md`.

## 7. 64 channels

The canonical packet remains eight channels per ADS1299. A 64-channel Android application should maintain:

- `deviceIndex` / slot identity;
- one parser/reassembly path per logical source as required;
- global channel mapping 1..64;
- per-device sequence/timestamp;
- bounded UI history.

Do not map channels by arrival order.

## 8. Run the smoke test

Compile/run the three Kotlin files with a Kotlin/JVM toolchain, or include the smoke-test logic in the project's unit-test source set.

The test exercises:

- signed int32 decode;
- CRC;
- arbitrary chunking;
- corruption recovery;
- sequence gaps;
- uint32 sequence wrap-around.

It is a software protocol test, not a claim of Android BLE/USB hardware validation.
