#!/usr/bin/env python3
"""Static acceptance checks for global platform rank 2."""

import json
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
TARGET = ROOT / "firmware" / "mcu" / "01_STMicroelectronics" / "STM32F407"
for relative in (
    "platformio.ini", "build.md", "sources.md",
    "stm32cube_adapter/library.json",
    "stm32cube_adapter/ads1299_stm32cube_hal.h",
    "stm32cube_adapter/ads1299_stm32cube_hal.c",
    "stm32cube_adapter/README.md",
    "examples/stm32cube_reference/main.c",
):
    path = TARGET / relative
    assert path.is_file() and path.stat().st_size > 0, f"missing {path}"

manifest = json.loads((TARGET / "project.json").read_text(encoding="utf-8"))
assert manifest["rank"] == 2 and manifest["tier"] == "A"
assert manifest["status"] == "Compiles"
assert manifest["board"] == "STM32F407G-DISC1 (MB997)"

adapter = "\n".join(
    path.read_text(encoding="utf-8")
    for path in (TARGET / "stm32cube_adapter").glob("*.[ch]")
)
assert not re.search(r"\b(?:CONFIG[1-4]|CH[1-8]SET|LOFF|BIAS_SENSP)\b", adapter)
assert "HAL_SPI_TransmitReceive" in adapter and "DWT->CYCCNT" in adapter

main = (TARGET / "examples" / "stm32cube_reference" / "main.c").read_text(encoding="utf-8")
assert "HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_SET)" in main
assert "ads1299_packet_encode" not in main and "HAL_UART" not in main

pinmap = (TARGET / "board" / "pinmap.md").read_text(encoding="utf-8")
for expected in ("PA5 / P1-15", "PA6 / P1-18", "PA7 / P1-17",
                 "PB0 / P1-22", "PB1 / P1-21"):
    assert expected in pinmap, f"missing official-board pin: {expected}"

platformio = (TARGET / "platformio.ini").read_text(encoding="utf-8")
assert "board = disco_f407vg" in platformio
assert "framework = stm32cube" in platformio
assert "file://../../../core_driver/ads1299" in platformio

legacy = TARGET / "lib" / "ads1299_port"
assert not legacy.exists() or not any(legacy.rglob("*")), "legacy duplicate port remains"

print("STM32F407 rank-2 package OK: official MB997, shared-SPI guard and clean layering")
