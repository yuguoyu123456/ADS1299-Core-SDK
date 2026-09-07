#!/usr/bin/env python3
"""Structural and layering checks for ranked ADS1299 FPGA references."""

from pathlib import Path
import hashlib
import json
import re


ROOT = Path(__file__).resolve().parents[1]
FPGA = ROOT / "firmware" / "fpga"
COMMON = (
    "README.md", "SOURCES.md", "integration.md", "validation.md",
    "rtl/ads1299_spi_master.v", "rtl/ads1299_controller.v",
    "rtl/frame_parser.v", "tb/simulation_tb.v",
)
FORBIDDEN_LOGIC = re.compile(
    r"\b(?:CONFIG[1-4]|CH[1-8]SET|BIAS_SENSP|ADS1299_REG_)\b"
)
FORBIDDEN_PARTS = {
    ".runs", ".cache", ".gen", ".hw", "db", "incremental_db", "output_files"
}


def digest(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main() -> None:
    errors: list[str] = []
    manifests = []
    for manifest_path in FPGA.rglob("project.json"):
        data = json.loads(manifest_path.read_text(encoding="utf-8"))
        manifests.append((manifest_path.parent, data))

    if len(manifests) != 15:
        errors.append(f"expected 15 FPGA packages, found {len(manifests)}")
    ranks = [data.get("rank") for _, data in manifests]
    if sorted(ranks) != list(range(71, 86)):
        errors.append(f"FPGA ranks must be exactly 71..85, got {sorted(ranks)}")

    reference_hashes: dict[str, str] = {}
    for index, (package, manifest) in enumerate(sorted(
            manifests, key=lambda item: item[1]["rank"])):
        relative_package = package.relative_to(FPGA).as_posix()
        if manifest.get("path") != relative_package or manifest.get("domain") != "fpga":
            errors.append(f"manifest path/domain mismatch: {relative_package}")
        required = (*COMMON, manifest.get("constraint", ""), "project.json")
        for relative in required:
            path = package / relative
            if not relative or not path.is_file() or path.stat().st_size == 0:
                errors.append(f"missing or empty: {path.relative_to(ROOT)}")

        validation = (package / "validation.md").read_text(encoding="utf-8")
        if "Current status: **Reference**" not in validation:
            errors.append(f"dishonest/unknown status: {package.relative_to(ROOT)}")

        for relative in ("rtl/ads1299_spi_master.v", "rtl/ads1299_controller.v"):
            text = (package / relative).read_text(encoding="utf-8")
            if FORBIDDEN_LOGIC.search(text):
                errors.append(f"ADS1299 register policy leaked into {package / relative}")

        parser = (package / "rtl/frame_parser.v").read_text(encoding="utf-8")
        if "CHANNEL_COUNT = 8" not in parser or "4'hC" not in parser:
            errors.append(f"variant/header checks missing: {package.relative_to(ROOT)}")

        for relative in COMMON[4:]:
            current = digest(package / relative)
            if index == 0:
                reference_hashes[relative] = current
            elif reference_hashes[relative] != current:
                errors.append(f"shared RTL drift: {package.relative_to(ROOT)}/{relative}")

    for path in FPGA.rglob("*"):
        if any(part.lower() in FORBIDDEN_PARTS for part in path.parts):
            errors.append(f"generated FPGA artifact present: {path.relative_to(ROOT)}")

    if errors:
        raise SystemExit("\n".join(errors))
    print("FPGA ecosystem OK: 15 ranked reference packages, shared RTL and clean layering")


if __name__ == "__main__":
    main()
