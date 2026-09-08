# TCP interoperability

ADS1299-Core uses the repository Ethernet block framing for reliable ordered TCP streaming.

The reference implementation is:

```text
firmware/transport/ethernet/ads1299_ethernet_frame.h
firmware/transport/ethernet/ads1299_ethernet_frame.c
```

## Block format

Each TCP application block begins with a fixed 16-byte header followed by 1..16 canonical ADS1299 packets.

Header layout:

```text
bytes 0..3    magic = 0x41 0x39 0x45 0x54  ("A9ET")
byte 4        version = 0
byte 5        flags
byte 6        packet_count (1..16)
byte 7        reserved = 0
bytes 8..9    payload_bytes, little-endian uint16
bytes 10..11  reserved = 0
bytes 12..15  block_sequence, little-endian uint32
```

Payload rule:

```text
payload_bytes = packet_count * 49
```

Total block size:

```text
16 + payload_bytes
```

Every contained 49-byte canonical packet must independently pass sync/version/CRC validation.

## TCP is a byte stream

One `send()` does not imply one receiver `recv()`. A receiver must:

1. collect enough bytes for the 16-byte header;
2. validate magic/version/count/payload length;
3. derive the full block size;
4. collect exactly that many bytes;
5. validate every contained canonical packet;
6. emit packets/epochs to the application.

Do not use TCP receive chunk boundaries as sample boundaries.

## 64-channel batching

One synchronized 8×ADS1299 epoch may be carried as eight canonical packets in one block:

```text
packet_count = 8
payload_bytes = 8 * 49 = 392
block_size = 16 + 392 = 408 bytes
```

This reduces application framing overhead while preserving one canonical packet per ADS1299 device.

## Reliability boundary

TCP provides ordered reliable byte delivery while a connection is alive, but the application still needs sequence counters because:

- acquisition-side queues may overflow;
- data may be lost before socket submission;
- reconnects create discontinuities;
- processes/devices may restart.

Canonical CRC remains useful for end-to-end format/integrity validation and storage/replay consistency.
