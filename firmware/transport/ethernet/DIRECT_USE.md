# Ethernet transport direct-use guide

This directory is additive. Existing files are retained unchanged.

## What this layer provides

`ads1299_ethernet_frame.[ch]` wraps one or more canonical 49-byte ADS1299 packets into one Ethernet transport block without changing the underlying packet format.

The block format is independent of lwIP, FreeRTOS+TCP, BSD sockets or a specific MAC/PHY driver. It can be used over either UDP datagrams or a TCP byte stream.

## Block layout

The fixed header is 16 bytes:

- bytes 0..3: ASCII-like magic `A9ET`
- byte 4: transport frame version
- byte 5: application flags
- byte 6: canonical packet count
- byte 7: reserved
- bytes 8..9: payload length, little-endian
- bytes 10..11: reserved
- bytes 12..15: Ethernet block sequence, little-endian
- bytes 16..: complete canonical ADS1299 packets

Each contained ADS1299 packet keeps its own sync, version, sample sequence, timestamp and CRC16.

Up to 16 canonical packets can be grouped in one block. The largest block is 800 bytes, which stays comfortably below a normal Ethernet MTU when carried in UDP/IP.

## Recommended 64-channel mapping

For 8 ADS1299 devices, one logical 64-channel sample point can be represented by 8 canonical packets and one Ethernet frame:

- 8 × 49 bytes packet payload = 392 bytes
- 16-byte Ethernet transport header
- 408 bytes total application payload before UDP/TCP/IP headers

Use a consistent device ordering, for example device index 0 through 7, and preserve packet sequence/timestamp information for each ADS1299.

## UDP use

UDP is useful when low latency is more important than guaranteed delivery.

1. Encode 1–8 packets with `ads1299_ethernet_frame_encode()`.
2. Send exactly one encoded frame as one UDP datagram.
3. On the receiver, require one complete datagram and call `ads1299_ethernet_frame_decode()`.
4. Use `block_sequence` plus each ADS1299 packet sequence to detect loss.
5. Do not retry from the DRDY path; loss accounting belongs in the transport layer.

Because the maximum application frame is 800 bytes, IP fragmentation should not be required on a normal 1500-byte MTU path.

## TCP use

TCP is useful when reliable ordered delivery is required.

TCP is a byte stream, so one `recv()` call is not guaranteed to return one ADS1299 Ethernet frame.

Receiver state machine:

1. Read exactly `ADS1299_ETH_FRAME_HEADER_SIZE` bytes.
2. Call `ads1299_ethernet_frame_peek_size()` to obtain the complete frame size.
3. Read the remaining bytes until that frame size is reached.
4. Call `ads1299_ethernet_frame_decode()`.
5. Repeat from the next 16-byte header.

Never assume TCP message boundaries match calls to `send()` or `recv()`.

## Acquisition/transport separation

Recommended architecture:

`DRDY ISR -> SPI/DMA acquisition -> bounded frame queue -> Ethernet task -> socket`

The DRDY ISR should only timestamp/signal work. The Ethernet stack, ARP, DHCP, TCP retransmission and PHY events must never run inside the acquisition ISR.

Track at least:

- ADS1299 sample sequence gaps;
- Ethernet `block_sequence` gaps;
- acquisition queue high-water mark;
- queue overflow count;
- socket send failures/back-pressure;
- reconnect count for TCP.

## Throughput

One ADS1299 at 250 SPS using 49-byte canonical packets produces about 12,250 bytes/s before Ethernet/IP overhead.

Eight ADS1299 devices at 250 SPS produce about 98,000 bytes/s of canonical packet data, which is easily within 100-Mbit Ethernet bandwidth. The important engineering concern is deterministic buffering and task scheduling, not raw link capacity.

## Validation

1. Run `ethernet_frame_smoke_test.c` with the common packet implementation.
2. Verify a frame containing one packet.
3. Verify a frame containing eight packets for the 64-channel case.
4. Corrupt one contained packet and confirm decode rejects it through the canonical packet CRC.
5. For TCP, force partial receives and verify the header-length state machine still reconstructs frames.
6. For UDP, intentionally drop datagrams and confirm `block_sequence` exposes the loss.
7. Hardware bring-up should start with ADS1299 internal test signal before real EEG electrodes.

## Board-specific items intentionally not fixed here

The SDK does not hard-code:

- Ethernet MAC instance;
- RMII/MII pins;
- PHY address;
- static IP address;
- TCP/UDP port number;
- lwIP/RTOS task priorities;
- cache/DMA placement rules for a specific MCU.

Those belong in each board/application port.
