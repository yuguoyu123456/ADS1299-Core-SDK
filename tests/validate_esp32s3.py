#!/usr/bin/env python3
"""Static acceptance checks for global platform rank 3."""

import json
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
TARGET = ROOT / "firmware" / "mcu" / "02_Espressif" / "ESP32S3"

for relative in (
    "platformio.ini",
    "build.md",
    "sources.md",
    "ads1299_port/CMakeLists.txt",
    "components/ads1299_esp_idf_hal/CMakeLists.txt",
    "components/ads1299_esp_idf_hal/ads1299_esp_idf_hal.h",
    "components/ads1299_esp_idf_hal/ads1299_esp_idf_hal.c",
    "main/main.c",
):
    path = TARGET / relative
    assert path.is_file() and path.stat().st_size > 0, f"missing {path}"

manifest = json.loads((TARGET / "project.json").read_text(encoding="utf-8"))
assert manifest["rank"] == 3 and manifest["tier"] == "A"
assert manifest["status"] == "Compiles"
assert manifest["board"] == "ESP32-S3-DevKitC-1-N8"

hal_text = "\n".join(
    path.read_text(encoding="utf-8")
    for path in (TARGET / "components" / "ads1299_esp_idf_hal").glob("*.[ch]")
)
assert not re.search(r"\b(?:CONFIG[1-4]|CH[1-8]SET|LOFF|BIAS_SENSP)\b", hal_text)
assert "ads1299_port_t" not in hal_text, "HAL must not bypass the generic MCU Port"
assert "ads1299_esp_idf_make_hal" in hal_text
assert "spi_device_polling_transmit" in hal_text

main = (TARGET / "main" / "main.c").read_text(encoding="utf-8")
for expected in ("SPI2_HOST", "GPIO_NUM_12", "GPIO_NUM_6",
                 "ads1299_mcu_port_init", "ads1299_read_frame_continuous"):
    assert expected in main, f"missing reference flow item: {expected}"
for forbidden in ("ads1299_packet_encode", "uart_write_bytes", "UART_NUM_0"):
    assert forbidden not in main, f"transport leaked into MCU example: {forbidden}"

pinmap = (TARGET / "board" / "pinmap.md").read_text(encoding="utf-8")
for expected in ("GPIO12 / J1-18", "GPIO11 / J1-17", "GPIO13 / J1-19",
                 "GPIO10 / J1-16", "GPIO9 / J1-15", "GPIO6 / J1-6"):
    assert expected in pinmap, f"missing official header mapping: {expected}"

platformio = (TARGET / "platformio.ini").read_text(encoding="utf-8")
assert "platform = platformio/espressif32@6.10.0" in platformio
assert "board = esp32-s3-devkitc-1" in platformio
assert "framework = espidf" in platformio
assert "src_dir = main" in platformio

legacy = TARGET / "components" / "ads1299_port"
assert not legacy.exists() or not any(legacy.rglob("*")), "duplicate legacy Port remains"

print("ESP32-S3 rank-3 package OK: layered IDF HAL, official headers and build entrypoint")
