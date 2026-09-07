#!/usr/bin/env python3
"""Acceptance checks for global platform rank 6."""
import json, re, shutil, subprocess, tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
TARGET = ROOT / "firmware/mcu/04_TexasInstruments/MSPM0G3507"
for name in ("build.md", "sources.md", "driverlib_adapter/README.md",
             "driverlib_adapter/ads1299_mspm0_hal.h",
             "driverlib_adapter/ads1299_mspm0_hal.c"):
    assert (TARGET/name).is_file(), f"missing {name}"
manifest = json.loads((TARGET/"project.json").read_text(encoding="utf-8"))
assert (manifest["rank"], manifest["status"], manifest["mcu"]) == (6, "Reference", "MSPM0G3507SPM")
source = (TARGET/"driverlib_adapter/ads1299_mspm0_hal.c").read_text(encoding="utf-8")
for api in ("DL_SPI_transmitData8", "DL_SPI_receiveData8", "DL_SPI_isBusy",
            "DL_GPIO_setPins", "DL_Common_delayCycles"):
    assert api in source
assert "ads1299_port_t" not in source
assert not re.search(r"\b(?:CONFIG[1-4]|CH[1-8]SET|LOFF|BIAS_SENSP)\b", source)
pins = (TARGET/"board/pinmap.md").read_text(encoding="utf-8")
for pin in ("PB9 / SPI1-SCK", "PB8 / SPI1-PICO", "PB7 / SPI1-POCI",
            "PB6 / SPI1-CS0 remuxed GPIO", "PA25 / GPIO input"):
    assert pin in pins, f"missing {pin}"

compiler = shutil.which("arm-none-eabi-gcc") or shutil.which("gcc")
pio = Path("E:/PlatformIO/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-gcc.exe")
if not compiler and pio.is_file(): compiler = str(pio)
if compiler:
    with tempfile.TemporaryDirectory(prefix="ads1299-mspm0-") as td:
        d = Path(td); inc = d/"ti/driverlib"; inc.mkdir(parents=True)
        (inc/"dl_gpio.h").write_text("#include <stdint.h>\ntypedef struct GPIO_Regs GPIO_Regs;\nvoid DL_GPIO_setPins(GPIO_Regs*,uint32_t);\nvoid DL_GPIO_clearPins(GPIO_Regs*,uint32_t);\nuint32_t DL_GPIO_readPins(GPIO_Regs*,uint32_t);\n", encoding="utf-8")
        (inc/"dl_spi.h").write_text("#include <stdint.h>\n#include <stdbool.h>\ntypedef struct SPI_Regs SPI_Regs;\nvoid DL_SPI_transmitData8(SPI_Regs*,uint8_t);\nuint8_t DL_SPI_receiveData8(SPI_Regs*);\nbool DL_SPI_isRXFIFOEmpty(SPI_Regs*);\nbool DL_SPI_isTXFIFOFull(SPI_Regs*);\nbool DL_SPI_isBusy(SPI_Regs*);\n", encoding="utf-8")
        (inc/"dl_common.h").write_text("#include <stdint.h>\nvoid DL_Common_delayCycles(uint32_t);\n", encoding="utf-8")
        out=d/"adapter.o"
        subprocess.run([compiler,"-std=c11","-Wall","-Wextra","-Werror",f"-I{d}",f"-I{TARGET/'ads1299_port'}",f"-I{ROOT/'firmware/core_driver/ads1299'}","-c",str(TARGET/"driverlib_adapter/ads1299_mspm0_hal.c"),"-o",str(out)],check=True)
        assert out.is_file()
    note="adapter fixture compile passed"
else: note="compiler unavailable; static checks passed"
print(f"MSPM0G3507 rank-6 package OK: layered DriverLib reference; {note}")
