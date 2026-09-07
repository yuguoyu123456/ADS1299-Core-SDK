#!/usr/bin/env python3
"""Validate the continuous 1-100 ADS1299 ecosystem index."""

from pathlib import Path
import json


ROOT = Path(__file__).resolve().parents[1]
FIRMWARE = ROOT / "firmware"


def main() -> None:
    unified = json.loads(
        (FIRMWARE / "ecosystem_catalog.json").read_text(encoding="utf-8")
    )["entries"]
    assert len(unified) == 100
    assert [item["rank"] for item in unified] == list(range(1, 101))
    assert sum(item["domain"] == "mcu" for item in unified) == 85
    assert sum(item["domain"] == "fpga" for item in unified) == 15
    assert all(item["domain"] == "fpga" for item in unified[70:85])
    assert all(item["domain"] == "mcu" for item in unified[:70] + unified[85:])
    for item in unified:
        target = FIRMWARE / item["path"]
        assert target.is_dir(), f"missing ranked directory: {target}"
        assert (target / "README.md").is_file(), f"missing ranked README: {target}"
    print("Ecosystem catalog OK: continuous ranks 1..100 (85 MCU + 15 FPGA)")


if __name__ == "__main__":
    main()
