"""Record ADS1299-Core host packets to CSV.

Example:
    python record_csv.py COM5 output.csv --baud 921600 --limit 10000
"""

from __future__ import annotations
import argparse
import csv
from serial_reader import packets_from_serial


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("port")
    ap.add_argument("output")
    ap.add_argument("--baud", type=int, default=921600)
    ap.add_argument("--limit", type=int, default=0, help="0 = unlimited")
    args = ap.parse_args()

    header = ["sequence", "timestamp_us", "status_hex"] + [f"ch{i}" for i in range(1, 9)]
    with open(args.output, "w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow(header)
        count = 0
        for frame in packets_from_serial(args.port, args.baud):
            writer.writerow([frame.sequence, frame.timestamp_us, frame.status.hex(), *frame.channels])
            count += 1
            if count % 250 == 0:
                f.flush()
                print(f"Recorded {count} frames")
            if args.limit and count >= args.limit:
                break


if __name__ == "__main__":
    main()
