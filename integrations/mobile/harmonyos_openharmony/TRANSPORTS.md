# HarmonyOS / OpenHarmony Transport Integration

The portable protocol core is intentionally independent from a specific HarmonyOS/OpenHarmony SDK version. Platform adapters should only be responsible for delivering bytes/fragments and lifecycle events.

## BLE

Repository BLE framing lives in `firmware/transport/ble/`.

A HarmonyOS/OpenHarmony BLE adapter should:

1. discover/connect using the Bluetooth APIs of the actual target SDK;
2. receive notifications/indications into a bounded pipeline;
3. parse the repository 8-byte BLE fragment header;
4. enforce packet sequence, fragment index/count and payload length;
5. reassemble exactly 49 canonical bytes;
6. call `Ads1299Protocol.decode()` to validate sync/version/CRC;
7. track device sequence gaps after canonical validation.

Do not assume one BLE notification is one ADS1299 packet. Sustained application payload depends on the real controller, connection and SDK stack and must be measured.

## TCP / raw network stream

If the source sends raw canonical packet bytes over TCP:

- feed every received `Uint8Array` chunk into one `Ads1299StreamParser`;
- allow a receive callback to contain partial or multiple packets;
- treat reconnect as an explicit session boundary;
- preserve parser/sequence counters.

If the sender uses the repository Ethernet/Wi-Fi block framing, decode the block first and validate each contained canonical 49-byte packet.

## WebSocket gateway

A HarmonyOS/OpenHarmony app may also consume the same gateway JSON used by `integrations/mobile/web_pwa/`.

Keep this as a separate path:

`WebSocket JSON -> JSON schema validation -> app model`

not:

`WebSocket JSON -> Ads1299StreamParser`.

The gateway should already have validated the source canonical CRC before emitting JSON.

## UART / serial bridge

Repository UART framing is COBS + `0x00` delimiter.

A serial bridge adapter should:

1. accumulate bytes until zero delimiter;
2. COBS-decode the frame;
3. require exactly one 49-byte canonical packet;
4. validate with `Ads1299Protocol.decode()`;
5. record sequence gaps and malformed-frame counts.

## USB

USB support varies with the actual HarmonyOS/OpenHarmony device, USB role and application APIs. Keep USB-specific code outside the protocol core.

If a supported USB adapter supplies a raw byte stream, use the same `Ads1299StreamParser`; USB read boundaries are not packet boundaries.

Do not claim generic USB compatibility until the exact phone/tablet/device combination has been validated.

## Back-pressure and threading

Use a bounded queue/channel between transport and recording/rendering. Track:

- queue high-water mark;
- overflow/drop count;
- parser/CRC errors;
- BLE fragment failures;
- network reconnects;
- device sequence gaps.

A visually smooth chart is not proof that the raw stream is complete.

## Lifecycle and background behavior

HarmonyOS/OpenHarmony background execution, permissions and BLE/network lifecycle behavior depend on the platform/version and app model. Validate the actual target instead of assuming desktop behavior.

If the OS suspends acquisition, record an explicit session gap. Never fabricate continuous EEG samples across a suspension/reconnect.

## 64-channel recommendation

For eight ADS1299 devices use an explicit pipeline:

`transport -> device/block identity -> canonical packet -> per-device integrity -> 64ch sample grouping -> recorder -> bounded/downsampled UI`

Prefer transports whose sustained throughput and latency margin have been measured for the target 64-channel sample rate.
