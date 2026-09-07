#!/usr/bin/env python3
"""Structural and layering checks for every visible MCU target."""

import json
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
MCU = ROOT / "firmware" / "mcu"
CATALOG = json.loads((MCU / "catalog.json").read_text(encoding="utf-8"))
PROJECTS = CATALOG.get("projects", [])
ALLOWED = {"Planned", "Reference", "Example", "Compatible", "Compiles", "Bench-tested", "24h-tested"}
REQUIRED = (
    "README.md", "platform_info.yaml", "capability.md", "project.json",
    "ads1299_port/ads1299_spi.c", "ads1299_port/ads1299_spi.h",
    "ads1299_port/ads1299_gpio.c", "ads1299_port/ads1299_gpio.h",
    "ads1299_port/ads1299_drdy.c", "ads1299_port/ads1299_drdy.h",
    "examples/main_ads1299.c", "board/pinmap.md", "integration.md",
    "version.md", "validation.md", "tests/test_spi.c", "tests/test_reset.c",
    "tests/test_register.c", "tests/test_frame.c",
)
FORBIDDEN_PORT_TOKENS = re.compile(
    r"\b(?:CONFIG[1-4]|CH[1-8]SET|BIAS_SENSP|BIAS_SENSN|LOFF|MISC1|GPIO_REG)\b"
)
REQUIRED_PORT_API = (
    "ads1299_mcu_spi_write", "ads1299_mcu_spi_read",
    "ads1299_mcu_spi_transfer", "ads1299_mcu_cs_low",
    "ads1299_mcu_cs_high", "ads1299_mcu_reset", "ads1299_mcu_start",
    "ads1299_mcu_data_ready", "ads1299_mcu_delay_us", "ads1299_mcu_delay_ms",
)


assert CATALOG.get("schema") == 2, "catalog schema must be 2"
assert len(PROJECTS) == 85, f"expected exactly 85 MCU/SoC/DSP targets, got {len(PROJECTS)}"
expected_ranks = set(range(1, 71)) | set(range(86, 101))
assert {item.get("rank") for item in PROJECTS} == expected_ranks, "MCU ranks must be 1..70 and 86..100"
seen = set()
for item in PROJECTS:
    relative = item["path"]
    assert relative not in seen, f"duplicate target path: {relative}"
    seen.add(relative)
    assert item["status"] in ALLOWED, f"invalid status for {relative}: {item['status']}"
    target = MCU / relative
    assert target.is_dir(), f"missing target directory: {target}"
    for required in REQUIRED:
        assert (target / required).is_file(), f"missing {target / required}"

    manifest = json.loads((target / "project.json").read_text(encoding="utf-8"))
    assert manifest.get("schema") == 2, f"old manifest schema: {relative}"
    assert manifest.get("path") == relative, f"manifest path mismatch: {relative}"
    assert manifest.get("status") == item["status"], f"status mismatch: {relative}"
    assert manifest.get("core") == "../../../core_driver/ads1299", f"bad Core path: {relative}"
    expected_tier = "A" if item["rank"] <= 20 else ("B" if item["rank"] <= 60 else "C")
    assert manifest.get("tier") == expected_tier, f"tier mismatch: {relative}"

    info = (target / "platform_info.yaml").read_text(encoding="utf-8")
    for key in ("rank", "vendor", "family", "mcu", "architecture", "board", "official_sdk",
                "sdk_version", "compiler", "debugger", "tier", "status"):
        assert re.search(rf"(?m)^{key}:\s*\S", info), f"missing YAML key {key}: {relative}"
    assert re.search(rf'(?m)^tier:\s*"?{expected_tier}"?\s*$', info), f"YAML tier mismatch: {relative}"

    validation = (target / "validation.md").read_text(encoding="utf-8")
    assert f"Current status: **{item['status']}**" in validation, f"validation mismatch: {relative}"
    if item["status"] == "Compiles":
        assert "Build entrypoint:" in validation, f"missing compile evidence: {relative}"

    port_text = "\n".join(
        p.read_text(encoding="utf-8", errors="strict")
        for p in sorted((target / "ads1299_port").glob("*.[ch]"))
    )
    match = FORBIDDEN_PORT_TOKENS.search(port_text)
    assert not match, f"ADS1299 business token {match.group(0)!r} found in port: {relative}"
    assert "ads1299_write_register" not in port_text, f"register API used in port: {relative}"
    assert "ads1299_configure" not in port_text, f"configuration API used in port: {relative}"
    for symbol in REQUIRED_PORT_API:
        assert symbol in port_text, f"missing port API {symbol}: {relative}"

    # Inspect shipping documents, not ignored tool caches or linked vendor SDKs.
    import os
    for folder, dirs, files in os.walk(target, followlinks=False):
        dirs[:] = [d for d in dirs if not d.startswith('.') and
                   d not in ('managed_components', 'build', '__pycache__') and
                   not Path(folder, d).is_symlink() and
                   not (hasattr(Path(folder, d), 'is_junction') and Path(folder, d).is_junction())]
        for name in files:
            if not name.endswith('.md'): continue
            markdown = Path(folder, name)
            text = markdown.read_text(encoding="utf-8")
            assert not re.search(r"(?m)^\+", text), f"patch marker leaked into {markdown}"

for dirname in ("21_Ambiq", "22_BouffaloLab", "23_Realtek", "24_Toshiba", "25_Other"):
    landing = MCU / dirname / "README.md"
    assert landing.is_file(), f"missing honest planned-vendor landing page: {landing}"
    assert "Directory role: **Catalog**" in landing.read_text(encoding="utf-8")

compiled = sum(item["status"] == "Compiles" for item in PROJECTS)
print(f"MCU ecosystem OK: {len(PROJECTS)} targets, {compiled} compile-evidenced, strict port layering passed")
