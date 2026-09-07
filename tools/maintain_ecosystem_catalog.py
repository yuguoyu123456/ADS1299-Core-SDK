#!/usr/bin/env python3
"""Build the unified ranked MCU/FPGA ecosystem catalog."""

from pathlib import Path
import json


ROOT = Path(__file__).resolve().parents[1]
FIRMWARE = ROOT / "firmware"


def write(path: Path, text: str) -> None:
    path.write_text(text.rstrip() + "\n", encoding="utf-8")


def main() -> None:
    mcu_catalog = json.loads(
        (FIRMWARE / "mcu" / "catalog.json").read_text(encoding="utf-8")
    )
    entries = []
    for item in mcu_catalog["projects"]:
        entries.append({
            "rank": item["rank"], "domain": "mcu", "vendor": item["vendor"],
            "family": item["family"], "path": f"mcu/{item['path']}",
            "status": item["status"],
        })
    for manifest_path in (FIRMWARE / "fpga").rglob("project.json"):
        item = json.loads(manifest_path.read_text(encoding="utf-8"))
        entries.append({
            "rank": item["rank"], "domain": "fpga", "vendor": item["vendor"],
            "family": item["family"], "path": f"fpga/{item['path']}",
            "status": item["status"],
        })
    entries.sort(key=lambda item: item["rank"])
    ranks = [item["rank"] for item in entries]
    if ranks != list(range(1, 101)):
        raise SystemExit(f"ecosystem ranks are not exactly 1..100: {ranks}")

    catalog = {
        "schema": 1,
        "policy": "Ranks 1-70 and 86-100 are MCU/SoC/DSP ports; 71-85 are FPGA interfaces.",
        "entries": entries,
    }
    write(FIRMWARE / "ecosystem_catalog.json",
          json.dumps(catalog, ensure_ascii=False, indent=2))

    lines = [
        "# ADS1299 ranked ecosystem: 1-100", "",
        "This table is generated from the maintained MCU and FPGA manifests.", "",
        "| Rank | Domain | Vendor | Family | Status | Path |",
        "|---:|---|---|---|---|---|",
    ]
    for item in entries:
        lines.append(
            f"| {item['rank']} | {item['domain']} | {item['vendor']} | "
            f"{item['family']} | {item['status']} | `{item['path']}` |"
        )
    write(FIRMWARE / "ECOSYSTEM_100.md", "\n".join(lines))
    print("Unified ecosystem OK: ranks 1..100, 85 MCU/SoC/DSP + 15 FPGA")


if __name__ == "__main__":
    main()
