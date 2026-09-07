#!/usr/bin/env python3
"""Acceptance checks for global platform rank 4."""

import json
import os
import re
import shutil
import subprocess
import tempfile
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
TARGET = ROOT / "firmware" / "mcu" / "03_NXP" / "MIMXRT1170"

for relative in (
    "build.md", "sources.md", "mcuxpresso_adapter/README.md",
    "mcuxpresso_adapter/ads1299_mcuxpresso_hal.h",
    "mcuxpresso_adapter/ads1299_mcuxpresso_hal.c",
):
    path = TARGET / relative
    assert path.is_file() and path.stat().st_size > 0, f"missing {path}"

manifest = json.loads((TARGET / "project.json").read_text(encoding="utf-8"))
assert manifest["rank"] == 4 and manifest["tier"] == "A"
assert manifest["status"] == "Reference"
assert manifest["mcu"] == "MIMXRT1176DVMAA"
assert manifest["board"] == "MIMXRT1170-EVKB"

adapter = "\n".join(
    path.read_text(encoding="utf-8")
    for path in (TARGET / "mcuxpresso_adapter").glob("*.[ch]")
)
assert "ads1299_mcuxpresso_make_hal" in adapter
assert "LPSPI_MasterTransferBlocking" in adapter
assert "ads1299_port_t" not in adapter
assert not re.search(r"\b(?:CONFIG[1-4]|CH[1-8]SET|LOFF|BIAS_SENSP)\b", adapter)

pinmap = (TARGET / "board" / "pinmap.md").read_text(encoding="utf-8")
for expected in ("GPIO_AD_28 / J10-12", "GPIO_AD_30 / J10-8",
                 "GPIO_AD_31 / J10-10", "Arduino D10 / J10-6",
                 "Arduino D2 / physical pin pending verification"):
    assert expected in pinmap, f"missing EVKB mapping: {expected}"

compiler = shutil.which("arm-none-eabi-gcc") or shutil.which("gcc")
if not compiler:
    pio = Path("E:/PlatformIO/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-gcc.exe")
    compiler = str(pio) if pio.is_file() else None

if compiler:
    with tempfile.TemporaryDirectory(prefix="ads1299-rt1170-") as temp_name:
        temp = Path(temp_name)
        (temp / "fsl_gpio.h").write_text(
            "#include <stdint.h>\ntypedef struct GPIO_Type GPIO_Type;\n"
            "void GPIO_PinWrite(GPIO_Type*, uint32_t, uint8_t);\n"
            "uint32_t GPIO_PinRead(GPIO_Type*, uint32_t);\n", encoding="utf-8")
        (temp / "fsl_lpspi.h").write_text(
            "#include <stddef.h>\n#include <stdint.h>\n"
            "typedef struct LPSPI_Type LPSPI_Type;\n"
            "typedef int status_t;\n#define kStatus_Success 0\n"
            "typedef struct { uint8_t *txData; uint8_t *rxData; size_t dataSize; uint32_t configFlags; } lpspi_transfer_t;\n"
            "status_t LPSPI_MasterTransferBlocking(LPSPI_Type*, lpspi_transfer_t*);\n",
            encoding="utf-8")
        (temp / "fsl_common.h").write_text(
            "#include <stdint.h>\nextern uint32_t SystemCoreClock;\n"
            "void SDK_DelayAtLeastUs(uint32_t, uint32_t);\n", encoding="utf-8")
        output = temp / "adapter.o"
        command = [compiler, "-std=c11", "-Wall", "-Wextra", "-Werror",
                   f"-I{temp}", f"-I{TARGET / 'ads1299_port'}",
                   f"-I{ROOT / 'firmware/core_driver/ads1299'}", "-c",
                   str(TARGET / "mcuxpresso_adapter/ads1299_mcuxpresso_hal.c"),
                   "-o", str(output)]
        subprocess.run(command, check=True, env={**os.environ, "PYTHONUTF8": "1"})
        assert output.is_file()
    compile_note = "adapter fixture compile passed"
else:
    compile_note = "compiler unavailable; static checks passed"

print(f"i.MX RT1170 rank-4 package OK: layered MCUXpresso reference; {compile_note}")
