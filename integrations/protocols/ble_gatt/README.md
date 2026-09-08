# BLE GATT interoperability

This directory defines the ADS1299-Core BLE application-layer contract without hard-coding one vendor SDK, phone OS, service UUID or connection configuration.

The firmware implementation lives in:

```text
firmware/transport/ble/
```

## Core principle

BLE notifications/indications are a transport layer around the same 49-byte ADS1299 canonical packet used elsewhere in the repository.

If one notification cannot carry the full canonical packet, the packet is fragmented with the repository BLE fragment header, reassembled on the receiver, then validated as a normal canonical packet.

## Repository BLE fragment header

Each fragment begins with an 8-byte header:

```text
byte 0      magic = 0xB9
bytes 1..4  packet_sequence, little-endian uint32
byte 5      fragment_index
byte 6      fragment_count
byte 7      payload_length
bytes 8..   canonical-packet fragment payload
```

Rules:

- fragment indices are zero-based;
- maximum fragment count is 16;
- fragments for a packet are processed in order by the reference reassembler;
- reconstructed canonical length must be exactly 49 bytes;
- canonical sync/version/CRC validation occurs after reassembly.

## No fixed UUIDs in the protocol core

Service and characteristic UUIDs are deployment/integration choices. Keep them in the platform adapter/configuration layer instead of embedding them in the canonical packet implementation.

A typical application still needs separate concepts for:

- data streaming;
- optional control/configuration;
- device information/status.

But this directory does not assign universal UUID values that the repository has not standardized.

## Throughput warning

BLE suitability depends on actual application payload, connection interval, controller/phone behavior, retries, RF environment and device count. Do not claim a channel/sample-rate combination from nominal BLE link speed alone.

For each canonical packet, fragmentation count is approximately:

```text
ceil(49 / (application_payload_max - 8))
```

when fragmentation is required and `application_payload_max > 8`.

## 64-channel systems

Eight ADS1299 devices generate eight canonical packets per sample epoch. Raw 64-channel streaming can create substantial BLE overhead. Preserve explicit source identity, apply bounded queues, and validate the exact target phone/controller/rate combination before choosing BLE as the primary raw-data transport.
