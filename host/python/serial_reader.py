"""Minimal serial stream reader for ADS1299-Core draft packets."""

from __future__ import annotations
import argparse
import serial
from packet import SYNC, PACKET_LEN, decode_packet


def packets_from_serial(port: str, baud: int):
    with serial.Serial(port, baudrate=baud, timeout=1) as ser:
        buf = bytearray()
        while True:
            chunk = ser.read(256)
            if not chunk:
                continue
            buf.extend(chunk)
            while True:
                pos = buf.find(SYNC)
                if pos < 0:
                    if len(buf) > 1:
                        del buf[:-1]
                    break
                if pos:
                    del buf[:pos]
                if len(buf) < PACKET_LEN:
                    break
                candidate = bytes(buf[:PACKET_LEN])
                try:
                    frame = decode_packet(candidate)
                except ValueError:
                    del buf[0]
                    continue
                del buf[:PACKET_LEN]
                yield frame


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("port")
    ap.add_argument("--baud", type=int, default=921600)
    args = ap.parse_args()
    last_seq = None
    for frame in packets_from_serial(args.port, args.baud):
        if last_seq is not None and frame.sequence != ((last_seq + 1) & 0xFFFFFFFF):
            print(f"WARN packet gap {last_seq} -> {frame.sequence}")
        last_seq = frame.sequence
        print(frame.sequence, frame.timestamp_us, *frame.channels)


if __name__ == "__main__":
    main()
