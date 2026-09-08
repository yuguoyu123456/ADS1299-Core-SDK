#!/usr/bin/env python3
"""Cross-platform ADS1299 canonical-packet recorder/validator.

Supported byte sources:
  - binary file replay
  - TCP stream
  - serial/USB CDC via optional pyserial
  - stdin

The parser accepts arbitrary byte chunk boundaries, searches for the canonical
0xA5 0x5A sync, validates version 0 and CRC16-CCITT-FALSE, and reports sequence
gaps. Raw counts are preserved by default.
"""

from __future__ import annotations

import argparse
import csv
import dataclasses
import socket
import struct
import sys
import time
from pathlib import Path
from typing import BinaryIO, Iterator, Optional

SYNC = b"\xA5\x5A"
VERSION = 0
PACKET_SIZE = 49
CHANNELS = 8
CRC_OFFSET = 47


@dataclasses.dataclass(frozen=True)
class Packet:
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
            if crc & 0x8000:
                crc = ((crc << 1) ^ 0x1021) & 0xFFFF
            else:
                crc = (crc << 1) & 0xFFFF
    return crc


def decode_packet(raw: bytes) -> Packet:
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
            f"CRC mismatch: wire=0x{actual_crc:04X} expected=0x{expected_crc:04X}"
        )

    flags = raw[3]
    sequence = int.from_bytes(raw[4:8], "little")
    timestamp_us = int.from_bytes(raw[8:12], "little")
    status = raw[12:15]
    channels = struct.unpack_from("<8i", raw, 15)
    return Packet(
        version=raw[2],
        flags=flags,
        sequence=sequence,
        timestamp_us=timestamp_us,
        status=status,
        channels=tuple(channels),
        crc=actual_crc,
        raw=raw,
    )


class StreamParser:
    """Incremental parser for arbitrary USB/TCP/serial/file byte chunks."""

    def __init__(self) -> None:
        self.buffer = bytearray()
        self.valid_packets = 0
        self.crc_or_format_errors = 0
        self.discarded_bytes = 0

    def feed(self, data: bytes) -> list[Packet]:
        if data:
            self.buffer.extend(data)
        out: list[Packet] = []

        while True:
            sync_index = self.buffer.find(SYNC)
            if sync_index < 0:
                # Keep one trailing 0xA5 because it might be the first sync byte
                # split across source reads.
                if self.buffer.endswith(SYNC[:1]):
                    dropped = max(0, len(self.buffer) - 1)
                    if dropped:
                        del self.buffer[:dropped]
                        self.discarded_bytes += dropped
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
                # Drop one byte, not a whole candidate, then search sync again.
                del self.buffer[0]
                self.discarded_bytes += 1
                self.crc_or_format_errors += 1
                continue

            del self.buffer[:PACKET_SIZE]
            self.valid_packets += 1
            out.append(packet)

        return out


class SequenceTracker:
    def __init__(self) -> None:
        self.last: Optional[int] = None
        self.gaps = 0
        self.missing_packets = 0
        self.duplicates_or_reordered = 0

    def observe(self, sequence: int) -> None:
        if self.last is None:
            self.last = sequence
            return

        expected = (self.last + 1) & 0xFFFFFFFF
        if sequence == expected:
            self.last = sequence
            return
        if sequence == self.last:
            self.duplicates_or_reordered += 1
            return

        forward = (sequence - expected) & 0xFFFFFFFF
        if forward < 0x80000000:
            self.gaps += 1
            self.missing_packets += forward
            self.last = sequence
        else:
            self.duplicates_or_reordered += 1


def code_to_microvolts(code: int, vref: float, gain: int) -> float:
    if vref <= 0.0 or gain <= 0:
        raise ValueError("vref and gain must be positive")
    return (float(code) * vref / (float(gain) * 8388607.0)) * 1_000_000.0


def iter_file(path: Path, chunk_size: int) -> Iterator[bytes]:
    with path.open("rb") as f:
        while True:
            chunk = f.read(chunk_size)
            if not chunk:
                break
            yield chunk


def iter_stdin(chunk_size: int) -> Iterator[bytes]:
    source = sys.stdin.buffer
    while True:
        chunk = source.read(chunk_size)
        if not chunk:
            break
        yield chunk


def iter_tcp(host: str, port: int, chunk_size: int, timeout: float) -> Iterator[bytes]:
    with socket.create_connection((host, port), timeout=timeout) as sock:
        sock.settimeout(timeout)
        while True:
            try:
                chunk = sock.recv(chunk_size)
            except socket.timeout:
                continue
            if not chunk:
                break
            yield chunk


def iter_serial(port: str, baud: int, chunk_size: int, timeout: float) -> Iterator[bytes]:
    try:
        import serial  # type: ignore
    except ImportError as exc:
        raise RuntimeError(
            "serial source requires pyserial: python -m pip install pyserial"
        ) from exc

    with serial.Serial(port=port, baudrate=baud, timeout=timeout) as ser:
        while True:
            chunk = ser.read(chunk_size)
            if chunk:
                yield bytes(chunk)


def open_csv(path: Optional[Path], units: str) -> tuple[Optional[BinaryIO], Optional[csv.writer]]:
    if path is None:
        return None, None
    text_file = path.open("w", newline="", encoding="utf-8")
    writer = csv.writer(text_file)
    suffix = "uv" if units == "uv" else "counts"
    writer.writerow(
        ["sequence", "timestamp_us", "flags", "status_hex"]
        + [f"ch{i + 1}_{suffix}" for i in range(CHANNELS)]
    )
    # Typing uses BinaryIO only to keep this file dependency-free at runtime;
    # the returned object is a normal text file and supports close/flush.
    return text_file, writer  # type: ignore[return-value]


def build_source(args: argparse.Namespace) -> Iterator[bytes]:
    if args.source == "file":
        return iter_file(Path(args.input), args.chunk_size)
    if args.source == "stdin":
        return iter_stdin(args.chunk_size)
    if args.source == "tcp":
        return iter_tcp(args.host, args.port, args.chunk_size, args.timeout)
    if args.source == "serial":
        return iter_serial(args.port, args.baud, args.chunk_size, args.timeout)
    raise ValueError(f"unknown source {args.source}")


def run(args: argparse.Namespace) -> int:
    parser = StreamParser()
    seq = SequenceTracker()
    csv_file = None
    csv_writer = None
    raw_file = None
    packet_limit = args.max_packets if args.max_packets > 0 else None
    start = time.monotonic()
    emitted = 0

    try:
        if args.csv:
            csv_file, csv_writer = open_csv(Path(args.csv), args.units)
        if args.raw:
            raw_file = Path(args.raw).open("wb")

        for chunk in build_source(args):
            for packet in parser.feed(chunk):
                seq.observe(packet.sequence)
                emitted += 1

                if raw_file is not None:
                    raw_file.write(packet.raw)

                if csv_writer is not None:
                    if args.units == "uv":
                        values = [
                            f"{code_to_microvolts(v, args.vref, args.gain):.9f}"
                            for v in packet.channels
                        ]
                    else:
                        values = list(packet.channels)
                    csv_writer.writerow(
                        [
                            packet.sequence,
                            packet.timestamp_us,
                            f"0x{packet.flags:02X}",
                            packet.status.hex().upper(),
                        ]
                        + values
                    )

                if args.print_every and emitted % args.print_every == 0:
                    elapsed = max(time.monotonic() - start, 1e-9)
                    print(
                        f"packets={emitted} rate={emitted / elapsed:.1f}/s "
                        f"seq={packet.sequence} gaps={seq.gaps} "
                        f"missing={seq.missing_packets} parser_errors={parser.crc_or_format_errors}",
                        file=sys.stderr,
                    )

                if packet_limit is not None and emitted >= packet_limit:
                    raise StopIteration

    except KeyboardInterrupt:
        pass
    except StopIteration:
        pass
    finally:
        if csv_file is not None:
            csv_file.flush()
            csv_file.close()
        if raw_file is not None:
            raw_file.flush()
            raw_file.close()

    elapsed = max(time.monotonic() - start, 1e-9)
    print(
        "final: "
        f"packets={emitted}, elapsed_s={elapsed:.3f}, rate={emitted / elapsed:.2f}/s, "
        f"gaps={seq.gaps}, missing={seq.missing_packets}, "
        f"duplicates_or_reordered={seq.duplicates_or_reordered}, "
        f"parser_errors={parser.crc_or_format_errors}, discarded_bytes={parser.discarded_bytes}",
        file=sys.stderr,
    )
    return 0 if parser.crc_or_format_errors == 0 and seq.missing_packets == 0 else 2


def add_common_arguments(p: argparse.ArgumentParser) -> None:
    p.add_argument("--chunk-size", type=int, default=4096)
    p.add_argument("--timeout", type=float, default=1.0)
    p.add_argument("--raw", help="write validated canonical packets to this binary file")
    p.add_argument("--csv", help="write decoded samples to CSV")
    p.add_argument("--units", choices=("counts", "uv"), default="counts")
    p.add_argument("--vref", type=float, default=4.5, help="reference voltage for --units uv")
    p.add_argument("--gain", type=int, default=24, help="PGA gain for --units uv")
    p.add_argument("--max-packets", type=int, default=0, help="0 means unlimited")
    p.add_argument("--print-every", type=int, default=250)


def main(argv: Optional[list[str]] = None) -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    sub = ap.add_subparsers(dest="source", required=True)

    p_file = sub.add_parser("file", help="replay/validate a binary byte stream")
    p_file.add_argument("input")
    add_common_arguments(p_file)

    p_stdin = sub.add_parser("stdin", help="read binary bytes from stdin")
    add_common_arguments(p_stdin)

    p_tcp = sub.add_parser("tcp", help="read canonical byte stream from TCP")
    p_tcp.add_argument("host")
    p_tcp.add_argument("port", type=int)
    add_common_arguments(p_tcp)

    p_serial = sub.add_parser("serial", help="read canonical byte stream from USB CDC/UART")
    p_serial.add_argument("port")
    p_serial.add_argument("--baud", type=int, default=921600)
    add_common_arguments(p_serial)

    args = ap.parse_args(argv)
    if args.chunk_size <= 0:
        ap.error("--chunk-size must be > 0")
    if args.units == "uv" and (args.vref <= 0.0 or args.gain <= 0):
        ap.error("--vref and --gain must be > 0 for microvolt conversion")

    try:
        return run(args)
    except (OSError, RuntimeError, ValueError) as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
