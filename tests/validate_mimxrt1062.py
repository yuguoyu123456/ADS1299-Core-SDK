#!/usr/bin/env python3
"""Acceptance checks for global platform rank 5."""

import json, re, shutil, subprocess, tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
TARGET = ROOT / "firmware/mcu/03_NXP/MIMXRT1062"

for name in ("build.md", "sources.md", "mcuxpresso_adapter/README.md",
             "mcuxpresso_adapter/ads1299_mcuxpresso_hal.h",
             "mcuxpresso_adapter/ads1299_mcuxpresso_hal.c"):
    assert (TARGET / name).is_file(), f"missing {name}"

manifest = json.loads((TARGET / "project.json").read_text(encoding="utf-8"))
assert (manifest["rank"], manifest["status"]) == (5, "Reference")
assert manifest["mcu"] == "MIMXRT1062DVL6B"
assert manifest["board"] == "MIMXRT1060-EVKB"

source = (TARGET / "mcuxpresso_adapter/ads1299_mcuxpresso_hal.c").read_text(encoding="utf-8")
assert "LPSPI_MasterTransferBlocking" in source
assert "ads1299_port_t" not in source
assert not re.search(r"\b(?:CONFIG[1-4]|CH[1-8]SET|LOFF|BIAS_SENSP)\b", source)

pins = (TARGET / "board/pinmap.md").read_text(encoding="utf-8")
for item in ("GPIO_SD_B0_00 / J17-6", "GPIO_SD_B0_02 / J17-4",
             "GPIO_SD_B0_03 / J17-5", "GPIO_SD_B0_01 / J17-3",
             "GPIO_AD_B0_11 / J16-3", "GPIO_AD_B1_08 / J16-4"):
    assert item in pins, f"missing official pin {item}"

compiler = shutil.which("arm-none-eabi-gcc") or shutil.which("gcc")
pio_gcc = Path("E:/PlatformIO/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-gcc.exe")
if not compiler and pio_gcc.is_file(): compiler = str(pio_gcc)
if compiler:
    with tempfile.TemporaryDirectory(prefix="ads1299-rt1062-") as td:
        fixture = Path(td)
        (fixture / "fsl_gpio.h").write_text("#include <stdint.h>\ntypedef struct GPIO_Type GPIO_Type;\nvoid GPIO_PinWrite(GPIO_Type*,uint32_t,uint8_t);\nuint32_t GPIO_PinRead(GPIO_Type*,uint32_t);\n", encoding="utf-8")
        (fixture / "fsl_lpspi.h").write_text("#include <stddef.h>\n#include <stdint.h>\ntypedef struct LPSPI_Type LPSPI_Type;\ntypedef int status_t;\n#define kStatus_Success 0\ntypedef struct{uint8_t*txData;uint8_t*rxData;size_t dataSize;uint32_t configFlags;}lpspi_transfer_t;\nstatus_t LPSPI_MasterTransferBlocking(LPSPI_Type*,lpspi_transfer_t*);\n", encoding="utf-8")
        (fixture / "fsl_common.h").write_text("#include <stdint.h>\nextern uint32_t SystemCoreClock;\nvoid SDK_DelayAtLeastUs(uint32_t,uint32_t);\n", encoding="utf-8")
        out = fixture / "adapter.o"
        subprocess.run([compiler, "-std=c11", "-Wall", "-Wextra", "-Werror",
            f"-I{fixture}", f"-I{TARGET/'ads1299_port'}",
            f"-I{ROOT/'firmware/core_driver/ads1299'}", "-c", str(TARGET/"mcuxpresso_adapter/ads1299_mcuxpresso_hal.c"), "-o", str(out)], check=True)
        assert out.is_file()
    note = "adapter fixture compile passed"
else:
    note = "compiler unavailable; static checks passed"

print(f"i.MX RT1060 rank-5 package OK: layered MCUXpresso reference; {note}")
