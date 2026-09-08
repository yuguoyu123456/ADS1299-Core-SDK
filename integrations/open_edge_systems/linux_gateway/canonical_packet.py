"""Dependency-light ADS1299-Core canonical packet decoder for edge gateways.

This module intentionally does not depend on pc_tools so Linux gateways can
validate and decode the repository-wide 49-byte packet locally.
"""

from __future__ import annotations

from dataclasses import dataclass
import struct

SYNC = b"\xA5\x5A"
VERSION = 0
PACKET_SIZE = 49
CHANNELS = 8
CRC_OFFSET = 47


@dataclass(frozen=True)
class CanonicalPacket:
    version: int
    flags: int
    sequence: int
    timestamp_us: int
    status: bytes
    channels: tuple[int, ...]
    crc: int
    raw: bytes


def crc16_ccitt_false(data: bytes) -> int:
    crc = 0xFFFF
    for byte in data:
        crc ^= byte << 8
        for _ in range(8):
            crc = (((crc << 1) ^ 0x1021) if (crc & 0x8000) else (crc << 1)) & 0xFFFF
    return crc


def decode_packet(raw: bytes) -> CanonicalPacket:
    if len(raw) != PACKET_SIZE:
        raise ValueError(f"packet size {len(raw)} != {PACKET_SIZE}")
    if raw[:2] != SYNC:
        raise ValueError("bad sync")
    if raw[2] != VERSION:
        raise ValueError(f"unsupported protocol version {raw[2]}")

    expected_crc = crc16_ccitt_false(raw[2:CRC_OFFSET])
    actual_crc = int.from_bytes(raw[CRC_OFFSET:CRC_OFFSET + 2], "little")
    if actual_crc != expected_crc:
        raise ValueError(
            f"CRC mismatch: wire=0x{actual_crc:04X}, expected=0x{expected_crc:04X}"
        )

    channels = struct.unpack_from("<8i", raw, 15)
    return CanonicalPacket(
        version=raw[2],
        flags=raw[3],
        sequence=int.from_bytes(raw[4:8], "little"),
        timestamp_us=int.from_bytes(raw[8:12], "little"),
        status=bytes(raw[12:15]),
        channels=tuple(channels),
        crc=actual_crc,
        raw=bytes(raw),
    )


class CanonicalStreamParser:
    """Recover validated 49-byte packets from arbitrary USB/TCP byte chunks."""

    def __init__(self) -> None:
        self.buffer = bytearray()
        self.valid_packets = 0
        self.format_or_crc_errors = 0
        self.discarded_bytes = 0

    def feed(self, data: bytes) -> list[CanonicalPacket]:
        if data:
            self.buffer.extend(data)
        out: list[CanonicalPacket] = []

        while True:
            sync_index = self.buffer.find(SYNC)
            if sync_index < 0:
                if self.buffer.endswith(SYNC[:1]):
                    drop = max(0, len(self.buffer) - 1)
                    if drop:
                        del self.buffer[:drop]
                        self.discarded_bytes += drop
                else:
                    self.discarded_bytes += len(self.buffer)
                    self.buffer.clear()
                break

            if sync_index:
                del self.buffer[:sync_index]
                self.discarded_bytes += sync_index

            if len(self.buffer) < PACKET_SIZE:
                break

            candidate = bytes(self.buffer[:PACKET_SIZE])
            try:
                packet = decode_packet(candidate)
            except ValueError:
                del self.buffer[0]
                self.discarded_bytes += 1
                self.format_or_crc_errors += 1
                continue

            del self.buffer[:PACKET_SIZE]
            self.valid_packets += 1
            out.append(packet)

        return out
