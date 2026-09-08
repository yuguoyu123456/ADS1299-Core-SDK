#!/usr/bin/env python3
"""Smoke tests for the dependency-light Linux gateway core modules."""

from __future__ import annotations

import struct

from canonical_packet import (
    CanonicalStreamParser,
    PACKET_SIZE,
    crc16_ccitt_false,
    decode_packet,
)
from multi_device import MultiDeviceEpochAssembler, global_channel_index


def make_packet(sequence: int, device_index: int = 0, timestamp_us: int = 123456) -> bytes:
    raw = bytearray(PACKET_SIZE)
    raw[0:2] = b"\xA5\x5A"
    raw[2] = 0
    raw[3] = 0x21
    raw[4:8] = (sequence & 0xFFFFFFFF).to_bytes(4, "little")
    raw[8:12] = (timestamp_us & 0xFFFFFFFF).to_bytes(4, "little")
    raw[12:15] = bytes((0xC0, device_index & 0x0F, 0x00))
    channels = tuple(device_index * 1000 + ch for ch in range(8))
    struct.pack_into("<8i", raw, 15, *channels)
    crc = crc16_ccitt_false(bytes(raw[2:47]))
    raw[47:49] = crc.to_bytes(2, "little")
    return bytes(raw)


def test_decode_and_stream_recovery() -> None:
    p1 = make_packet(10)
    p2 = make_packet(11)
    decoded = decode_packet(p1)
    assert decoded.sequence == 10
    assert decoded.channels[7] == 7

    bad = bytearray(make_packet(99))
    bad[20] ^= 0x01

    parser = CanonicalStreamParser()
    output = []
    stream = b"garbage" + p1 + bytes(bad) + b"noise" + p2
    for cut in (3, 17, 5, 64, 2, 1000):
        if not stream:
            break
        chunk, stream = stream[:cut], stream[cut:]
        output.extend(parser.feed(chunk))
    if stream:
        output.extend(parser.feed(stream))

    assert [p.sequence for p in output] == [10, 11]
    assert parser.valid_packets == 2
    assert parser.format_or_crc_errors >= 1
    assert parser.discarded_bytes > 0


def test_eight_device_epoch_out_of_order() -> None:
    assembler = MultiDeviceEpochAssembler(device_count=8, max_pending_epochs=4)
    epoch = None
    for device_index in (7, 2, 5, 0, 6, 1, 4, 3):
        packet = decode_packet(make_packet(42, device_index, timestamp_us=1000 + device_index))
        result = assembler.push(device_index, packet)
        if result is not None:
            epoch = result

    assert epoch is not None
    assert epoch.sequence == 42
    assert epoch.device_count == 8
    assert len(epoch.channels) == 64
    assert epoch.channels[global_channel_index(0, 0)] == 0
    assert epoch.channels[global_channel_index(3, 4)] == 3004
    assert epoch.channels[global_channel_index(7, 7)] == 7007
    assert epoch.timestamp_us == tuple(1000 + i for i in range(8))
    assert assembler.completed_epochs == 1
    assert assembler.pending_epochs == 0


def test_duplicate_and_bounded_pending() -> None:
    assembler = MultiDeviceEpochAssembler(device_count=2, max_pending_epochs=2)
    p = decode_packet(make_packet(1, 0))
    assert assembler.push(0, p) is None
    assert assembler.push(0, p) is None
    assert assembler.duplicate_device_packets == 1

    # Create incomplete epochs 2, 3, 4. Capacity is 2, so one must be evicted.
    for sequence in (2, 3, 4):
        assembler.push(0, decode_packet(make_packet(sequence, 0)))
    assert assembler.pending_epochs == 2
    assert assembler.evicted_epochs >= 1


def main() -> None:
    test_decode_and_stream_recovery()
    test_eight_device_epoch_out_of_order()
    test_duplicate_and_bounded_pending()
    print("linux gateway core smoke tests: PASS")


if __name__ == "__main__":
    main()
