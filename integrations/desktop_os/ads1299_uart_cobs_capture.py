#!/usr/bin/env python3
"""Capture repository UART transport (COBS + 0x00) and emit canonical packets.

This helper is intentionally separate from ads1299_desktop_capture.py because
USB/TCP may carry raw canonical packet bytes while firmware/transport/uart uses
COBS framing with a zero delimiter.
"""

from __future__ import annotations

import argparse
import csv
import sys
from pathlib import Path
from typing import Optional

from ads1299_desktop_capture import Packet, SequenceTracker, decode_packet


def cobs_decode(frame: bytes) -> bytes:
    if not frame:
        raise ValueError("empty COBS frame")

    out = bytearray()
    index = 0
    length = len(frame)

    while index < length:
        code = frame[index]
        if code == 0:
            raise ValueError("zero byte inside COBS frame")
        index += 1
        copy_len = code - 1
        if index + copy_len > length:
            raise ValueError("truncated COBS frame")
        out.extend(frame[index:index + copy_len])
        index += copy_len
        if code != 0xFF and index < length:
            out.append(0)

    return bytes(out)


def capture(
    port: str,
    baud: int,
    timeout: float,
    raw_path: Optional[Path],
    csv_path: Optional[Path],
    max_packets: int,
) -> int:
    try:
        import serial  # type: ignore
    except ImportError as exc:
        raise RuntimeError(
            "UART capture requires pyserial: python -m pip install pyserial"
        ) from exc

    raw_file = raw_path.open("wb") if raw_path else None
    csv_file = csv_path.open("w", newline="", encoding="utf-8") if csv_path else None
    writer = csv.writer(csv_file) if csv_file else None
    if writer:
        writer.writerow(
            ["sequence", "timestamp_us", "flags", "status_hex"]
            + [f"ch{i + 1}_counts" for i in range(8)]
        )

    tracker = SequenceTracker()
    frame = bytearray()
    valid = 0
    bad_cobs = 0
    bad_packet = 0

    try:
        with serial.Serial(port=port, baudrate=baud, timeout=timeout) as ser:
            while max_packets <= 0 or valid < max_packets:
                chunk = ser.read(4096)
                if not chunk:
                    continue
                for byte in chunk:
                    if byte != 0:
                        frame.append(byte)
                        # Bound memory if delimiter is lost.
                        if len(frame) > 256:
                            frame.clear()
                            bad_cobs += 1
                        continue

                    if not frame:
                        continue

                    encoded = bytes(frame)
                    frame.clear()
                    try:
                        canonical = cobs_decode(encoded)
                    except ValueError:
                        bad_cobs += 1
                        continue

                    try:
                        packet: Packet = decode_packet(canonical)
                    except ValueError:
                        bad_packet += 1
                        continue

                    tracker.observe(packet.sequence)
                    valid += 1
                    if raw_file:
                        raw_file.write(packet.raw)
                    if writer:
                        writer.writerow(
                            [
                                packet.sequence,
                                packet.timestamp_us,
                                f"0x{packet.flags:02X}",
                                packet.status.hex().upper(),
                            ]
                            + list(packet.channels)
                        )

                    if valid % 250 == 0:
                        print(
                            f"packets={valid} gaps={tracker.gaps} missing={tracker.missing_packets} "
                            f"bad_cobs={bad_cobs} bad_packet={bad_packet}",
                            file=sys.stderr,
                        )
    except KeyboardInterrupt:
        pass
    finally:
        if raw_file:
            raw_file.flush()
            raw_file.close()
        if csv_file:
            csv_file.flush()
            csv_file.close()

    print(
        f"final: packets={valid} gaps={tracker.gaps} missing={tracker.missing_packets} "
        f"duplicates_or_reordered={tracker.duplicates_or_reordered} "
        f"bad_cobs={bad_cobs} bad_packet={bad_packet}",
        file=sys.stderr,
    )
    return 0 if bad_cobs == 0 and bad_packet == 0 and tracker.missing_packets == 0 else 2


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("port", help="COMx on Windows or /dev/tty* on macOS/Linux")
    ap.add_argument("--baud", type=int, default=921600)
    ap.add_argument("--timeout", type=float, default=1.0)
    ap.add_argument("--raw", type=Path, help="write validated 49-byte canonical packets")
    ap.add_argument("--csv", type=Path, help="write decoded raw counts")
    ap.add_argument("--max-packets", type=int, default=0, help="0 means unlimited")
    args = ap.parse_args()

    try:
        return capture(
            port=args.port,
            baud=args.baud,
            timeout=args.timeout,
            raw_path=args.raw,
            csv_path=args.csv,
            max_packets=args.max_packets,
        )
    except (OSError, RuntimeError, ValueError) as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
