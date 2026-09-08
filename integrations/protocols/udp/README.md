# UDP interoperability

ADS1299-Core can carry the same `A9ET` Ethernet block used for TCP inside one UDP datagram.

The reference block encoder/decoder is:

```text
firmware/transport/ethernet/ads1299_ethernet_frame.h
firmware/transport/ethernet/ads1299_ethernet_frame.c
```

## Recommended datagram contract

Use:

```text
one UDP datagram = one complete A9ET block
```

The 16-byte block header contains:

- magic `A9ET`;
- block version;
- flags;
- packet count 1..16;
- payload byte count;
- uint32 block sequence.

The payload contains `packet_count` complete 49-byte canonical ADS1299 packets.

Maximum repository block size is:

```text
16 + 16 * 49 = 800 bytes
```

A synchronized 8×ADS1299 / 64-channel epoch can use eight packets:

```text
16 + 8 * 49 = 408 bytes/datagram
```

## Why keep the block header on UDP

Even though UDP preserves datagram boundaries, the application block still provides:

- explicit packet count;
- application block sequence;
- format/version check;
- consistent payload layout shared with TCP;
- independent canonical CRC checks for every ADS1299 packet.

## UDP reliability model

UDP applications must explicitly tolerate:

- dropped datagrams;
- duplicated datagrams;
- reordered datagrams;
- variable latency/jitter;
- receiver queue overflow.

Use `block_sequence` and canonical packet `sequence` values to detect these conditions. Never assume datagram arrival order equals acquisition order.

## 64-channel identity

If one datagram represents one eight-device epoch, packet ordering inside that datagram must be deterministic or source identity must be carried separately by the integration layer.

Do not fill a missing device packet using data from a later datagram merely to maintain 64 values.
