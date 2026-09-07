#!/usr/bin/env python3
"""Static acceptance checks for global platform rank 1."""

import json
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
TARGET = ROOT / "firmware" / "mcu" / "01_STMicroelectronics" / "STM32H743"
REQUIRED = (
    "platformio.ini",
    "build.md",
    "sources.md",
    "stm32cube_adapter/library.json",
    "stm32cube_adapter/ads1299_stm32cube_hal.h",
    "stm32cube_adapter/ads1299_stm32cube_hal.c",
    "stm32cube_adapter/README.md",
    "examples/stm32cube_reference/main.c",
)

for relative in REQUIRED:
    path = TARGET / relative
    assert path.is_file() and path.stat().st_size > 0, f"missing {path}"

manifest = json.loads((TARGET / "project.json").read_text(encoding="utf-8"))
assert manifest["rank"] == 1
assert manifest["status"] == "Compiles"
assert manifest["tier"] == "A"

adapter = "\n".join(
    path.read_text(encoding="utf-8")
    for path in (TARGET / "stm32cube_adapter").glob("*.[ch]")
)
forbidden = re.compile(r"\b(?:CONFIG[1-4]|CH[1-8]SET|LOFF|BIAS_SENSP|BIAS_SENSN)\b")
assert not forbidden.search(adapter), "ADS1299 policy leaked into STM32Cube adapter"
assert "HAL_SPI_TransmitReceive" in adapter
assert "DWT->CYCCNT" in adapter

pinmap = (TARGET / "board" / "pinmap.md").read_text(encoding="utf-8")
for expected in ("PA5 / CN7 D13", "PA7 / CN7 D11", "SB33 ON, SB35 OFF",
                 "PA6 / CN7 D12", "PD14 / CN7 D10", "PF3 / CN7 D8"):
    assert expected in pinmap, f"missing verified pin condition: {expected}"

platformio = (TARGET / "platformio.ini").read_text(encoding="utf-8")
assert "framework = stm32cube" in platformio
assert "board = nucleo_h743zi" in platformio
assert "file://../../../core_driver/ads1299" in platformio

print("STM32H743 rank-1 package OK: Cube adapter, ZI2 pin evidence and build entrypoint")
