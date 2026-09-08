# BLE transport direct-use guide

This directory is additive. Existing files are retained unchanged.

## Purpose

The canonical ADS1299-Core-SDK packet is 49 bytes. BLE ATT payload size can be smaller than 49 bytes, so the transport must not assume one ADS1299 packet always fits in one notification.

`ads1299_ble_frag.[ch]` provides SDK-independent packet fragmentation and reassembly. The BLE stack itself remains platform-specific.

## Fragment format

Each BLE fragment starts with an 8-byte header:

- byte 0: magic `0xB9`
- bytes 1..4: packet sequence, little-endian
- byte 5: fragment index, starting at 0
- byte 6: fragment count
- byte 7: payload length
- remaining bytes: fragment payload

The original 49-byte ADS1299 packet remains unchanged inside the fragments. Its own sync/version/CRC are checked after reassembly.

## Sender flow

1. Produce one normal 49-byte ADS1299 packet using `ads1299_packet_encode()` or `ads1299_transport_send_sample()` upstream.
2. Determine the maximum notification payload available from the active BLE connection. Pass that value as `att_payload_max`; do not hard-code one universal MTU.
3. Call `ads1299_ble_fragment_count(ADS1299_PACKET_SIZE, att_payload_max)`.
4. For fragment index `0..count-1`, call `ads1299_ble_make_fragment()`.
5. Queue notifications from a BLE/transport task. Do not transmit from the ADS1299 DRDY ISR.
6. If the BLE stack reports busy/no buffers, keep acquisition independent and use a bounded ring buffer. Count dropped packets when the producer outruns the link.

For the common 20-byte ATT notification payload, the 8-byte fragment header leaves 12 payload bytes, so one 49-byte packet uses 5 fragments.

## Receiver flow

1. Initialize one `ads1299_ble_reassembler_t` per connection with `ads1299_ble_reassembler_init()`.
2. Feed each received notification to `ads1299_ble_reassembler_push()`.
3. Return value `0`: fragment accepted, packet not complete yet.
4. Return value `1`: one complete 49-byte ADS1299 packet is available in `packet_out` and its packet CRC has validated.
5. Negative return: malformed, missing, out-of-order or CRC-invalid packet. Drop that packet and wait for the next fragment index 0.
6. Monitor `completed_packets` and `dropped_packets` for field diagnostics.

## Throughput rules

BLE is normally the most bandwidth-constrained backend in this repository. For EEG streaming:

- negotiate the largest practical MTU/data length supported by both peers;
- use notifications rather than indication-per-fragment when the application can tolerate application-layer loss detection;
- keep the connection interval short enough for the target sample rate;
- batch only outside the DRDY real-time path;
- keep sequence numbers end-to-end so packet loss is visible at the host;
- do not silently discard back-pressure events.

At 250 SPS, the canonical packet payload alone is `49 * 250 = 12,250 bytes/s` before BLE fragmentation and link overhead. Multi-ADS1299 systems multiply that traffic. A 64-channel design should normally aggregate on a higher-bandwidth transport (USB/Ethernet/Wi-Fi) or reduce/pack data before choosing BLE as the primary stream.

## Validation order

1. Unit-test fragmentation/reassembly on the host with `ble_frag_smoke_test.c`.
2. Verify one 49-byte packet at the minimum connection payload you intend to support.
3. Inject a missing or reordered fragment and confirm `dropped_packets` increments.
4. Corrupt one byte and confirm final ADS1299 packet CRC rejects it.
5. On hardware, stream internal ADS1299 test signal first.
6. Log BLE queue high-water mark, link busy count, sequence gaps and reassembly drops.
7. Only after zero-loss baseline operation move to EEG inputs and higher sample rates.

## Integration boundary

This module intentionally does not contain vendor BLE APIs, UUIDs, fixed characteristic handles, radio parameters or board pin numbers. Map its fragment output to the BLE SDK used by the selected MCU. This keeps the ADS1299 packet protocol identical across Nordic, Espressif, TI, Silicon Labs, Realtek, Telink and other BLE-capable targets.
