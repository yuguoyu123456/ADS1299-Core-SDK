#!/usr/bin/env python3
"""Dependency-free smoke tests for desktop packet and COBS parsers."""

from ads1299_desktop_capture import (
    PACKET_SIZE,
    SequenceTracker,
    StreamParser,
    crc16_ccitt_false,
    decode_packet,
)
from ads1299_uart_cobs_capture import cobs_decode


def make_packet(sequence: int, timestamp_us: int) -> bytes:
    out = bytearray(PACKET_SIZE)
    out[0:2] = b"\xA5\x5A"
    out[2] = 0
    out[3] = 0x21
    out[4:8] = sequence.to_bytes(4, "little")
    out[8:12] = timestamp_us.to_bytes(4, "little")
    out[12:15] = bytes((0xC0, 0x00, 0x00))
    values = (-8388608, -123456, -1, 0, 1, 123456, 4194304, 8388607)
    for i, value in enumerate(values):
        out[15 + i * 4:19 + i * 4] = int(value).to_bytes(4, "little", signed=True)
    crc = crc16_ccitt_false(bytes(out[2:47]))
    out[47:49] = crc.to_bytes(2, "little")
    return bytes(out)


def cobs_encode(data: bytes) -> bytes:
    out = bytearray()
    code_index = 0
    out.append(0)
    code = 1

    for byte in data:
        if byte == 0:
            out[code_index] = code
            code_index = len(out)
            out.append(0)
            code = 1
        else:
            out.append(byte)
            code += 1
            if code == 0xFF:
                out[code_index] = code
                code_index = len(out)
                out.append(0)
                code = 1

    out[code_index] = code
    return bytes(out)


def test_decode() -> None:
    raw = make_packet(7, 123456)
    packet = decode_packet(raw)
    assert packet.sequence == 7
    assert packet.timestamp_us == 123456
    assert packet.channels[0] == -8388608
    assert packet.channels[-1] == 8388607


def test_stream_arbitrary_chunks() -> None:
    p0 = make_packet(10, 1000)
    p1 = make_packet(11, 2000)
    parser = StreamParser()
    received = []
    stream = b"junk" + p0 + p1
    cuts = (1, 2, 7, 13, 29, 5, 64, 128)
    offset = 0
    for size in cuts:
        received.extend(parser.feed(stream[offset:offset + size]))
        offset += size
    received.extend(parser.feed(stream[offset:]))
    assert [p.sequence for p in received] == [10, 11]
    assert parser.discarded_bytes >= 4


def test_crc_resync() -> None:
    bad = bytearray(make_packet(20, 1000))
    bad[25] ^= 0x01
    good = make_packet(21, 2000)
    parser = StreamParser()
    received = parser.feed(bytes(bad) + good)
    assert [p.sequence for p in received] == [21]
    assert parser.crc_or_format_errors >= 1


def test_sequence_gap() -> None:
    tracker = SequenceTracker()
    tracker.observe(100)
    tracker.observe(101)
    tracker.observe(104)
    assert tracker.gaps == 1
    assert tracker.missing_packets == 2


def test_cobs_roundtrip() -> None:
    raw = make_packet(99, 777)
    encoded = cobs_encode(raw)
    assert b"\x00" not in encoded
    decoded = cobs_decode(encoded)
    assert decoded == raw
    assert decode_packet(decoded).sequence == 99


def main() -> int:
    test_decode()
    test_stream_arbitrary_chunks()
    test_crc_resync()
    test_sequence_gap()
    test_cobs_roundtrip()
    print("desktop integration smoke tests: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
