#!/usr/bin/env python3
"""Check the transport boundary without claiming any hardware backend."""

import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
TRANSPORT = ROOT / "firmware" / "transport"
REQUIRED = (
    "README.md",
    "CMakeLists.txt",
    "ads1299_transport.h",
    "ads1299_transport.c",
    "uart/README.md",
    "usb/README.md",
    "ble/README.md",
    "wifi/README.md",
    "ethernet/README.md",
    "lsl/README.md",
)
FORBIDDEN = re.compile(
    r"\b(?:CONFIG[1-4]|CH[1-8]SET|BIAS_SENSP|BIAS_SENSN|LOFF|MISC1)\b"
)

for relative in REQUIRED:
    path = TRANSPORT / relative
    assert path.is_file(), f"missing transport file: {path}"
    assert path.stat().st_size > 0, f"empty transport file: {path}"

source = "\n".join(
    path.read_text(encoding="utf-8")
    for path in TRANSPORT.glob("*.[ch]")
)
match = FORBIDDEN.search(source)
assert not match, f"ADS1299 register policy leaked into transport: {match.group(0)}"
assert "ads1299_write_register" not in source
assert "ads1299_transport_send_sample" in source

print("Transport layer OK: common packet sink plus 6 honest backend boundaries")
