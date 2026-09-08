# TCP validation checklist

## Framing

- [ ] 16-byte header is parsed independently of `recv()` chunk boundaries;
- [ ] magic is exactly `A9ET`;
- [ ] version is accepted only when supported;
- [ ] packet count is 1..16;
- [ ] `payload_bytes == packet_count * 49`;
- [ ] block sequence is decoded little-endian;
- [ ] every contained canonical packet passes sync/version/CRC validation.

## Stream behavior

Test these receive patterns:

- [ ] one complete block per receive;
- [ ] header split across several receives;
- [ ] payload split across many receives;
- [ ] several blocks coalesced in one receive;
- [ ] connection closes in the middle of a header;
- [ ] connection closes in the middle of a payload;
- [ ] malformed header/magic;
- [ ] valid block header containing one bad canonical packet.

## Backpressure and reconnect

- [ ] partial socket writes are handled;
- [ ] sender queue is bounded;
- [ ] queue overflow behavior is explicit;
- [ ] reconnect clears partial connection state;
- [ ] sequence discontinuities remain visible after reconnect;
- [ ] a slow receiver cannot stall DRDY acquisition.

## 64-channel

- [ ] an 8-packet epoch produces `packet_count=8` and `payload_bytes=392`;
- [ ] complete block size is 408 bytes;
- [ ] device ordering/source identity is deterministic;
- [ ] missing source packets are not silently filled from the next epoch;
- [ ] sustained sample-rate throughput is measured on the real target network/host.

## Qualification record

Record exact:

- sender hardware/firmware;
- receiver OS/application;
- network interface/path;
- sample rate;
- ADS1299 device count;
- block batching policy;
- duration;
- sequence gaps;
- reconnect events;
- queue high-water/overflow counters;
- CPU/memory where relevant.

Passing this checklist validates the TCP transport path only, not analog EEG quality or hardware safety.
