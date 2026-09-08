# USB transport direct-use guide

This directory is additive. Existing files are retained unchanged.

## Design rule

USB transfer boundaries are not application packet boundaries.

Whether the device uses USB CDC ACM, vendor-specific bulk endpoints or another stream-like USB class, the host may receive the same transmitted bytes in different chunk sizes. Therefore the host must reconstruct canonical ADS1299 packets from the byte stream rather than assume one `read()` equals one packet.

`ads1299_usb_stream.[ch]` provides:

- validation and concatenation of 1..16 canonical 49-byte packets for TX batching;
- byte-stream recovery based on canonical `0xA5 0x5A` sync, fixed 49-byte size and CRC16;
- automatic resynchronization after corruption/noise;
- counters for valid packets, invalid candidates and discarded bytes.

## Device transmit path

Recommended architecture:

`DRDY -> SPI/DMA capture -> canonical packet queue -> USB TX task -> endpoint`

For one ADS1299, a device may submit one or several 49-byte packets per USB transfer.

For eight ADS1299 devices, one logical 64-channel sample point can be batched as eight canonical packets:

`8 * 49 = 392 bytes`

Call `ads1299_usb_batch_encode()` to validate and concatenate the packets. The output is simply the existing packet stream; no extra USB-specific header is required.

Do not block inside the DRDY ISR waiting for USB endpoint availability.

## Host receive path

1. Initialize `ads1299_usb_stream_decoder_t`.
2. Read any number of bytes from the OS USB/serial API.
3. Feed every byte to `ads1299_usb_stream_decoder_push()`.
4. Return `1` means one complete canonical packet has been reconstructed and CRC-validated.
5. Continue feeding the remainder of the current host read buffer; one read may contain zero, one or many ADS1299 packets.

Track:

- `packets_ok`;
- `packets_bad`;
- `bytes_discarded`;
- host-side sequence gaps;
- device-side USB queue overflow count.

## CDC ACM versus vendor bulk

### CDC ACM

Advantages:

- simple driver availability on Windows/Linux/macOS;
- convenient development/debugging;
- easy integration with serial-port tooling.

Treat CDC as a binary byte stream. Do not convert the EEG stream to ASCII CSV for normal operation.

### Vendor-specific bulk

Advantages:

- less serial-class abstraction;
- flexible endpoint layout;
- often better control over buffering/latency.

Requires a host driver/API choice such as WinUSB/libusb or platform-specific APIs.

The canonical packet stream remains identical for both approaches.

## Endpoint packet size

For USB Full Speed, a bulk endpoint commonly uses a 64-byte maximum packet size. A 49-byte ADS1299 packet may fit inside one USB packet, but that fact must not become an application framing assumption.

A 392-byte 8-device batch naturally spans multiple USB packets. That is normal.

Do not depend on USB short packets or zero-length packets as the only ADS1299 application-frame delimiter. The packet sync/size/CRC parser remains authoritative.

## Throughput

Canonical data rate at 250 SPS:

- one ADS1299: about 12,250 bytes/s;
- eight ADS1299: about 98,000 bytes/s.

That is well within normal USB Full-Speed bulk/CDC capability. Production issues are more likely to come from endpoint scheduling, small queues, blocking firmware, host read latency or cache/DMA mistakes than from raw USB line rate.

Higher SPS scales linearly.

## Latency versus batching

Larger USB batches reduce per-transfer overhead but add buffering latency.

For interactive EEG:

- begin with 1 logical sample point per transfer or a small fixed batch;
- measure end-to-end latency and CPU load;
- increase batching only when needed;
- never delay acquisition waiting to fill a large batch.

## 64-channel ordering

When sending eight canonical packets per logical sample point, keep a stable device order and preserve each packet's sequence/timestamp.

If strict simultaneous sampling matters, synchronize ADS1299 conversion/clocking before USB transport. USB only transports acquired samples; it cannot correct ADC skew after acquisition.

## Validation

1. Run `usb_stream_smoke_test.c` with the common packet implementation.
2. Verify 1-packet and 8-packet batches.
3. Feed the decoder host reads split at awkward boundaries such as 1, 7, 63, 64 and 65 bytes.
4. Prefix random noise and verify resynchronization.
5. Corrupt one packet and confirm the next valid packet is recovered.
6. Stream ADS1299 internal test signal for an extended run while logging sequence gaps and queue high-water marks.
7. Test on the actual Windows/Linux host stack intended for deployment.

## Board-specific items intentionally not fixed here

The SDK does not hard-code USB VID/PID, endpoint numbers, descriptors, CDC instance, TinyUSB/ST USB/NXP USB stack configuration or DMA memory placement. Those remain board/application responsibilities.
