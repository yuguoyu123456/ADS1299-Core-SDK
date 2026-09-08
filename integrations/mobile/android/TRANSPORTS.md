# Android Transport Integration

The protocol parser is transport-independent. Transport code must deliver bytes/fragments without changing canonical packet semantics.

## BLE

Firmware BLE framing is defined in `firmware/transport/ble/`.

Important rules:

- one BLE notification is not automatically one ADS1299 packet;
- use the actual application notification payload size;
- parse the repository 8-byte BLE fragment header;
- enforce packet sequence, fragment index and fragment count;
- reassemble exactly 49 canonical bytes;
- validate canonical CRC after reassembly;
- keep BLE callbacks short and move parsing/recording off the callback thread where appropriate.

If Android BLE APIs deliver callbacks on a platform-managed thread, do not block them with chart rendering or synchronous file writes.

## USB

For USB host/accessory paths that expose a raw canonical byte stream:

- USB read boundaries are arbitrary;
- feed all received chunks to one `Ads1299StreamParser` instance;
- do not require 49 bytes per USB read;
- preserve sequence/CRC statistics.

For bulk transfers, batch sizes can contain several complete packets or partial packets at either end.

## Wi-Fi / TCP

For a raw canonical TCP stream:

- feed socket chunks directly to `Ads1299StreamParser`;
- keep socket reconnection as an explicit session boundary;
- do not reset loss statistics silently on reconnect.

If the device sends the repository Ethernet/Wi-Fi transport block format, decode that block first and then pass each contained canonical packet to `Ads1299Protocol.decode()`.

## WebSocket JSON

If Android consumes the same gateway JSON as `mobile/web_pwa`, the canonical CRC has already been checked by the gateway. The app should still validate:

- message type;
- channel count;
- sequence;
- timestamp;
- status formatting;
- device identity for multi-device streams.

Do not mix raw-binary and gateway-JSON schemas in one parser path.

## UART / USB-serial

Repository UART transport uses COBS + `0x00` delimiter. A mobile USB-serial integration must:

1. accumulate until zero delimiter;
2. COBS-decode;
3. require the decoded payload to be one canonical 49-byte packet;
4. validate canonical CRC;
5. track sequence gaps.

## Back-pressure

Mobile transport producers and recorders run at different latencies. Use a bounded queue/channel with an explicit overload policy.

Record:

- queue high-water mark;
- dropped buffers/packets;
- reconnect count;
- parser/CRC failures;
- sequence gaps.

A UI that remains smooth while silently dropping data is not an acceptable recorder.

## 64-channel recommendation

For 8 ADS1299 devices, keep transport/device identity explicit. One practical model is:

`transport source -> device/block decoder -> canonical packet -> device index -> 64ch group/recorder -> downsampled UI`

Prefer USB or measured high-throughput Wi-Fi/TCP for full-rate 64-channel raw recording. BLE can remain useful for lower channel counts, lower rates or control/telemetry depending on measured sustained throughput.
