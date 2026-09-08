# HarmonyOS / OpenHarmony integration

This directory provides the platform-neutral protocol core and integration guidance for using ADS1299-Core data from HarmonyOS / OpenHarmony applications.

The design intentionally keeps the **49-byte ADS1299 canonical packet** independent from any one release of HarmonyOS BLE, networking, UI, or device SDK APIs. Platform adapters should deliver transport bytes into the protocol layer instead of duplicating packet parsing logic inside UI or Bluetooth callbacks.

## Included files

- `Ads1299Protocol.ts` — canonical 49-byte packet constants, CRC16-CCITT-FALSE validation and packet decoding.
- `Ads1299StreamParser.ts` — incremental byte-stream parser with sync recovery for USB/TCP/raw canonical streams.
- `Ads1299ProtocolSmokeTest.ts` — dependency-light protocol/parser smoke test.
- `DIRECT_USE.md` — recommended application architecture and integration sequence.
- `TRANSPORTS.md` — BLE, USB, TCP/WebSocket and UART transport boundaries.
- `VALIDATION.md` — platform and protocol validation checklist.

## Canonical packet contract

The shared wire packet remains the repository-wide format:

- bytes `0..1`: sync `0xA5 0x5A`
- byte `2`: protocol version
- byte `3`: flags
- bytes `4..7`: sequence, little-endian `uint32`
- bytes `8..11`: timestamp in microseconds, little-endian `uint32`
- bytes `12..14`: ADS1299 status bytes
- bytes `15..46`: eight sign-extended channel samples as little-endian `int32`
- bytes `47..48`: CRC16-CCITT-FALSE over bytes `2..46`, little-endian on wire

Do not silently reinterpret this format inside the application layer. BLE fragmentation, UART COBS framing and TCP/WebSocket transport are outer transport layers; they must be removed/reassembled before the canonical parser receives bytes.

## Recommended application pipeline

```text
BLE / USB / TCP / WebSocket / UART adapter
                |
                v
transport reassembly / deframing
                |
                v
Ads1299StreamParser / Ads1299Protocol
                |
                v
validated Packet objects
                |
        +-------+--------+
        |                |
        v                v
recording / export   UI / signal display
```

Keep acquisition parsing off the UI rendering path when sustained data rates are high. Use a bounded queue or equivalent application architecture between transport callbacks and visualization/recording work.

## BLE note

The repository BLE transport may fragment one canonical packet across multiple ATT application payloads. Do not pass each BLE notification directly to `Ads1299Protocol.decode()` unless that notification is already one complete canonical packet. Reassemble the BLE transport fragments first, then decode the resulting 49 bytes.

## UART note

The repository UART transport uses COBS framing terminated by `0x00`. COBS decode first, then feed the recovered 49-byte canonical packet to this protocol layer.

## 64-channel systems

Eight ADS1299 devices produce eight canonical packets per synchronized sample epoch. Preserve at minimum:

- source/device index;
- packet sequence;
- device timestamp;
- deterministic mapping from device/channel to global channel index.

A common mapping is `global_channel = device_index * 8 + local_channel`, where both indices are zero-based. Do not infer synchronization merely from arrival order over BLE or Wi-Fi.

## Validation status

The files in this directory define reusable protocol logic and integration guidance. They do **not** claim validation on every HarmonyOS/OpenHarmony release, phone/tablet model, BLE controller, USB accessory path or application framework version. Use `VALIDATION.md` for target-specific qualification.

No medical-safety, EMC, electrical-isolation or regulatory compliance claim is implied by software-level packet validation.
