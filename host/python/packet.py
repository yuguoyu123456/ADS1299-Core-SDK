"""Parser for the draft ADS1299-Core host packet protocol."""

from __future__ import annotations
from dataclasses import dataclass
import struct

SYNC = b"\xA5\x5A"
PACKET_LEN = 49
BODY_LEN_WITHOUT_CRC = 45


def crc16_ccitt(data: bytes, init: int = 0xFFFF) -> int:
    crc = init
    for byte in data:
        crc ^= byte << 8
        for _ in range(8):
            crc = ((crc << 1) ^ 0x1021) & 0xFFFF if (crc & 0x8000) else (crc << 1) & 0xFFFF
    return crc


@dataclass
class Frame:
    version: int
    flags: int
    sequence: int
    timestamp_us: int
    status: bytes
    channels: tuple[int, ...]


def decode_packet(packet: bytes) -> Frame:
    if len(packet) != PACKET_LEN:
        raise ValueError(f"Expected {PACKET_LEN} bytes, got {len(packet)}")
    if packet[:2] != SYNC:
        raise ValueError("Bad sync word")

    body = packet[2:-2]
    expected = int.from_bytes(packet[-2:], "little")
    actual = crc16_ccitt(body)
    if actual != expected:
        raise ValueError(f"CRC mismatch: expected 0x{expected:04X}, got 0x{actual:04X}")

    version, flags, sequence, timestamp_us = struct.unpack_from("<BBII", body, 0)
    status = body[10:13]
    channels = struct.unpack_from("<8i", body, 13)
    return Frame(version, flags, sequence, timestamp_us, status, channels)


def encode_packet(frame: Frame) -> bytes:
    if len(frame.status) != 3 or len(frame.channels) != 8:
        raise ValueError("Need 3 status bytes and 8 channels")
    body = struct.pack("<BBII", frame.version, frame.flags, frame.sequence, frame.timestamp_us)
    body += bytes(frame.status)
    body += struct.pack("<8i", *frame.channels)
    crc = crc16_ccitt(body)
    return SYNC + body + crc.to_bytes(2, "little")
