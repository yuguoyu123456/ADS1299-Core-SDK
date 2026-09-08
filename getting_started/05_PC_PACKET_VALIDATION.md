# 05 - PC Packet Validation

Use this after the device can stream continuously. The goal is to prove that transport and host parsing preserve every sample.

## Canonical packet

The repository's common packet is fixed at 49 bytes and includes:

- sync bytes `0xA5 0x5A`;
- protocol/version/flags fields;
- sequence number;
- timestamp;
- 3 ADS1299 status bytes;
- 8 channels represented as signed 32-bit values after 24-bit sign extension;
- CRC16.

Use the implementation in `firmware/common/data_packet/` as the source of truth.

## Host validation order

For every received packet:

1. locate/confirm sync bytes;
2. require exactly the expected packet size at the canonical-packet layer;
3. validate CRC;
4. verify protocol/version fields;
5. check sequence continuity;
6. decode timestamp and status;
7. decode channels in the documented order;
8. only then convert counts to physical units or write analysis files.

## Sequence-gap accounting

Do not silently interpolate or duplicate packets in the raw recorder.

Record at least:

- packets received;
- CRC failures;
- sequence gaps;
- duplicate/out-of-order sequence numbers;
- transport reconnects;
- host-buffer overflows if applicable.

If a presentation/plotter wants continuous-looking data, keep any interpolation in a separate visualization layer and preserve the original loss markers.

## Transport-specific notes

### USB

USB reads may return arbitrary byte counts. Use the USB stream parser in `firmware/transport/usb/`; do not assume one host read equals one packet.

### UART

Use the COBS/zero-delimited framing helper in `firmware/transport/uart/`, then validate the recovered 49-byte packet CRC.

### BLE

Reassemble BLE fragments first with `firmware/transport/ble/ads1299_ble_frag.*`, then validate the canonical packet.

### Ethernet/Wi-Fi

For UDP/TCP block transport, decode the transport block first and validate each contained canonical packet. The Ethernet framing helper can aggregate multiple 49-byte packets without changing their contents.

### LSL

LSL should be a host-side bridge after packet validation. Keep the original device sequence/timestamp available for audit even if the LSL outlet uses host-local timing.

## Raw-file recommendation

A robust recorder should preserve enough information to reconstruct the acquisition exactly. At minimum keep either:

- the original validated 49-byte packets; or
- a lossless table containing sequence, timestamp, status, all raw channel counts and capture metadata.

Do not store only a filtered plot image or only microvolt values rounded for display.

## Basic automated checks

A host smoke test should fail when any of the following occurs:

- one byte is corrupted and CRC no longer matches;
- a sequence number is skipped;
- channel order changes;
- a signed negative 24-bit sample is decoded as a large positive number;
- a transport frame claims an impossible length;
- data-rate metadata disagrees with observed sample cadence.

## Pass criteria

Proceed to multi-device scaling only after a long single-device capture shows stable packet cadence, no unexplained CRC failures and an understood/recorded count for every sequence discontinuity.
