# 64-Channel Throughput Budget

Always budget from the actual encoded representation used by the system.

## 1. ADS1299 raw SPI payload

One ADS1299-8 continuous frame is 27 bytes:

- 3 status bytes
- 8 channels × 3 bytes

For `N` devices at sample rate `Fs`:

`raw_spi_bytes_per_second = 27 × N × Fs`

For eight ADS1299 devices:

| Sample rate | Raw ADS1299 bytes/s | Raw payload bit/s |
|---:|---:|---:|
| 250 SPS | 54,000 | 432,000 |
| 500 SPS | 108,000 | 864,000 |
| 1000 SPS | 216,000 | 1,728,000 |

This is only frame payload. SPI also needs command/CS timing, inter-device gaps and design margin.

## 2. SDK canonical packet payload

The repository canonical packet is 49 bytes per ADS1299 sample.

For `N` devices:

`packet_bytes_per_second = 49 × N × Fs`

For eight devices / 64 channels:

| Sample rate | Canonical bytes/s | Canonical bit/s |
|---:|---:|---:|
| 250 SPS | 98,000 | 784,000 |
| 500 SPS | 196,000 | 1,568,000 |
| 1000 SPS | 392,000 | 3,136,000 |

These values are before USB/UART/BLE/TCP/UDP/IP/link-layer overhead.

## 3. Why packet size is larger than raw frame

The canonical packet intentionally adds information needed for robust recording:

- sync/protocol fields;
- sequence number;
- timestamp;
- status;
- sign-extended channel storage;
- CRC.

Do not remove integrity/timing metadata merely to make a marginal transport appear fast enough.

## 4. UART budget

UART 8N1 typically needs ten serial bits per payload byte before any higher-level framing.

COBS framing adds small data-dependent overhead plus a delimiter.

Therefore calculate:

`required_uart_baud > encoded_bytes_per_second × serial_bits_per_byte × margin`

A baud rate that barely equals the average payload requirement is not adequate. Include ISR/DMA gaps, host scheduling and the selected COBS framing.

For full 64-channel raw streaming, validate that the actual MCU UART and host adapter sustain the selected rate continuously before choosing UART as the primary transport.

## 5. BLE budget

BLE throughput depends strongly on the controller, PHY, connection interval, negotiated data length/MTU and stack behavior.

The repository BLE helper fragments each 49-byte canonical packet when necessary. Fragment headers and notifications add overhead.

Do not assume nominal BLE PHY rate equals sustained application payload throughput. Measure the real target and keep a bounded queue with explicit overflow counters.

## 6. USB budget

USB is generally a better fit for deterministic high-channel-count streaming than low-baud UART or constrained BLE.

The repository USB helper can batch multiple complete 49-byte packets. For one 64-channel sample group:

`8 × 49 = 392 bytes`

Batching reduces software/transfer overhead while preserving canonical packet CRC/sequence information.

Still validate host read behavior and back-pressure under long-duration recording.

## 7. Ethernet / Wi-Fi budget

The Ethernet framing helper can aggregate 1-16 canonical packets per block.

For 64 channels, grouping eight device packets per sample gives one 392-byte canonical payload group plus the transport block header.

Ethernet/Wi-Fi usually provide ample nominal bandwidth, but network jitter and socket blocking are still acquisition risks. Keep networking behind a bounded queue and never block the DRDY-critical path.

## 8. LSL budget

LSL should normally run on the host after validated packets have arrived. LSL throughput is therefore a host-processing/network synchronization concern, not a replacement for reliable device transport.

Preserve raw device sequence/timestamps even when converting to an LSL stream.

## 9. Buffer budget

If the transport can stall for `Tstall` seconds, the minimum data buffer required before overflow is approximately:

`buffer_bytes >= encoded_bytes_per_second × Tstall`

Then add design margin and account for packet/block granularity.

Measure the real worst-case stall. Do not allocate a large queue and assume the problem is solved.

## 10. Acceptance test

At the target sample rate/device count:

- measure actual encoded bytes/s;
- record queue high-water mark;
- inject/observe realistic host/network stalls;
- verify overflow counters;
- verify packet sequence/CRC after a long capture;
- verify acquisition continues when lower-priority transport is busy.

A transport is suitable only when the measured system has sustained-throughput and latency margin, not merely because its advertised link speed is high enough.
