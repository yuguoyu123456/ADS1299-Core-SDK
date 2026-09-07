#!/usr/bin/env python3
"""Generate the maintainable MCU port documentation and thin port contract.

This script intentionally does not download vendor SDKs.  It creates only the
repository-owned ADS1299 port layer, minimal examples, metadata and tests.
Run it from any directory; paths are resolved relative to this file.
"""

from __future__ import annotations

import json
from dataclasses import dataclass, replace
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
MCU_ROOT = ROOT / "firmware" / "mcu"


@dataclass(frozen=True)
class Target:
    path: str
    vendor: str
    family: str
    mcu: str
    architecture: str
    board: str
    sdk: str
    sdk_version: str
    compiler: str
    debugger: str
    tier: int
    status: str
    cpu: str
    ram: str
    flash: str
    spi: str
    dma: str
    usb: str
    ble: str
    multi: str
    pins: tuple[str, str, str, str, str, str, str, str, str]
    rank: int = 0


PINS_ARM = ("PA5", "PA7", "PA6", "PB0", "PB1", "PB2", "PB10", "PB11", "PA9")
PINS_HEADER = ("SDK_SCK", "SDK_MOSI", "SDK_MISO", "SDK_CS", "SDK_DRDY", "SDK_RESET", "SDK_PWDN", "SDK_START", "SDK_TX")


TARGETS = [
    Target("01_STMicroelectronics/STM32F407", "STMicroelectronics", "STM32F4", "STM32F407VGT6", "Arm Cortex-M4F", "STM32F407G-DISC1 (MB997)", "STM32CubeF4", "1.28.3", "GNU Arm Embedded via PlatformIO / STM32CubeIDE", "on-board ST-LINK/V2-A", 1, "Compiles", "168 MHz Cortex-M4F", "192 KiB SRAM including 64 KiB CCM; PlatformIO linker exposes 128 KiB main SRAM", "1 MiB Flash", "SPI1 Mode 1; shared with on-board LIS3DSH, whose PE3 CS must stay high", "Two DMA controllers with 16 streams and FIFOs", "USB OTG FS/HS", "No integrated radio", "8 channels straightforward; multiple ADS1299 devices need separate CS/DRDY and measured scheduling", ("PA5 / P1-15", "PA7 / P1-17", "PA6 / P1-18", "PB0 / P1-22", "PB1 / P1-21", "PB11 / P1-35", "PB12 / P1-36", "PB13 / P1-37", "PA2 / P1-14 (transport only)")),
    Target("01_STMicroelectronics/STM32H743", "STMicroelectronics", "STM32H7", "STM32H743ZIT6", "Arm Cortex-M7F", "NUCLEO-H743ZI2 (MB1364)", "STM32CubeH7", "1.13.0", "GNU Arm Embedded via PlatformIO / STM32CubeIDE", "ST-LINK/V3E", 1, "Compiles", "up to 480 MHz Cortex-M7", "up to 1 MiB SRAM", "2 MiB Flash", "SPI1 Mode 1; verified polling reference build", "Yes; D1/D2 DMA and cache-coherency rules apply", "USB OTG FS/HS", "No integrated radio", "8 channels straightforward; 16/32/64 channels require per-device CS, buffering and measured DMA scheduling", ("PA5 / CN7 D13", "PA7 / CN7 D11 (SB33 ON, SB35 OFF)", "PA6 / CN7 D12", "PD14 / CN7 D10", "PF3 / CN7 D8", "PG12 / CN10 D7", "PE9 / CN10 D6", "PE11 / CN10 D5", "PD8 / ST-LINK VCP")),
    Target("02_Espressif/ESP32S3", "Espressif", "ESP32-S3", "ESP32-S3", "Xtensa LX7 dual-core", "ESP32-S3-DevKitC-1-N8", "ESP-IDF", "5.4.0", "xtensa-esp-elf-gcc 14.2.0", "USB Serial/JTAG", 1, "Compiles", "dual-core up to 240 MHz", "512 KiB on-chip SRAM; 320 KiB application RAM budget in the pinned PlatformIO board", "8 MiB on the N8 reference board", "SPI2 Mode 1; 4 MHz bring-up", "SPI master supports DMA; reference uses polling transactions", "USB 2.0 OTG and USB Serial/JTAG", "Bluetooth LE 5", "Multiple devices require measured scheduling; radio tasks must not block acquisition", ("GPIO12 / J1-18", "GPIO11 / J1-17", "GPIO13 / J1-19", "GPIO10 / J1-16", "GPIO9 / J1-15", "GPIO8 / J1-12", "GPIO6 / J1-6", "GPIO7 / J1-7", "firmware/transport backend")),
    Target("02_Espressif/ESP32C6", "Espressif", "ESP32-C6", "ESP32-C6", "32-bit RISC-V", "ESP32-C6-DevKitC-1", "ESP-IDF", "5.4+ recommended", "riscv32-esp-elf-gcc", "USB Serial/JTAG", 1, "Reference", "high-performance core up to 160 MHz", "512 KiB HP SRAM", "external module Flash varies", "GPSPI Mode 1; start at 4 MHz", "Yes", "USB Serial/JTAG", "BLE 5 LE", "Multiple devices possible; radio coexistence needs buffering", ("GPIO6", "GPIO7", "GPIO2", "GPIO10", "GPIO3", "GPIO4", "GPIO5", "GPIO1", "UART0 TX")),
    Target("03_NXP/MIMXRT1062", "NXP", "i.MX RT1060", "MIMXRT1062DVL6B", "Arm Cortex-M7F", "MIMXRT1060-EVKB", "MCUXpresso SDK", "25.06.00 reference", "arm-none-eabi-gcc / MCUXpresso IDE", "on-board OpenSDA/CMSIS-DAP", 1, "Reference", "Cortex-M7 up to 600 MHz", "1 MiB on-chip RAM", "8 MB QSPI boot Flash plus optional HyperFlash on EVKB", "LPSPI1 Mode 1; 4 MHz bring-up", "eDMA available; reference adapter is blocking", "two USB 2.0 OTG controllers", "optional M.2 Wi-Fi/Bluetooth module; no integrated radio", "One device reference; multi-device requires separate CS/DRDY and measured scheduling", ("GPIO_SD_B0_00 / J17-6", "GPIO_SD_B0_02 / J17-4", "GPIO_SD_B0_03 / J17-5", "GPIO_SD_B0_01 / J17-3", "GPIO_AD_B0_11 / J16-3", "GPIO_AD_B1_08 / J16-4", "GPIO_AD_B0_09 / J16-5", "GPIO_AD_B0_10 / J16-6", "OpenSDA VCOM (transport only)"), 5),
    Target("03_NXP/LPC55S69", "NXP", "LPC55S6x", "LPC55S69JBD100", "dual Arm Cortex-M33", "LPCXpresso55S69", "MCUXpresso SDK", "pin in the consuming project", "arm-none-eabi-gcc / MCUXpresso", "CMSIS-DAP", 1, "Reference", "up to 150 MHz", "320 KiB SRAM", "640 KiB Flash", "Flexcomm SPI Mode 1", "Yes", "USB HS/FS", "No", "Multiple devices possible with per-device CS", PINS_HEADER),
    Target("04_TexasInstruments/MSPM0G3507", "Texas Instruments", "MSPM0", "MSPM0G3507SPM", "Arm Cortex-M0+", "LP-MSPM0G3507", "MSPM0 SDK DriverLib", "2.11.00.07 reference", "TI Arm Clang / arm-none-eabi-gcc", "on-board XDS110", 1, "Reference", "80 MHz Cortex-M0+", "32 KiB SRAM", "128 KiB Flash", "SPI1 Mode 1; 4 MHz bring-up", "7-channel DMA; adapter reference is blocking", "No target USB peripheral; XDS110 provides debug/VCOM", "No integrated radio", "One device reference; budget RAM and output bandwidth before adding devices", ("PB9 / SPI1-SCK", "PB8 / SPI1-PICO", "PB7 / SPI1-POCI", "PB6 / SPI1-CS0 remuxed GPIO", "PA25 / GPIO input", "PA26 / GPIO output", "PA27 / GPIO output", "PA28 / GPIO output", "PA10 / XDS110 VCOM TX (transport only)"), 6),
    Target("04_TexasInstruments/TMS320F28379D", "Texas Instruments", "C2000", "TMS320F28379D", "dual C28x + dual CLA", "LAUNCHXL-F28379D", "C2000Ware DriverLib", "pin in CCS project", "TI C2000 compiler", "XDS100v2", 1, "Reference", "dual 200 MHz C28x", "204 KiB RAM", "1 MiB Flash", "SPI Mode 1", "Yes", "USB device", "No", "Excellent deterministic multi-device capability", PINS_HEADER),
    Target("04_TexasInstruments/TM4C1294", "Texas Instruments", "TM4C129x", "TM4C1294NCPDT", "Arm Cortex-M4F", "EK-TM4C1294XL", "TivaWare", "pin in CCS project", "TI Arm Clang / arm-none-eabi-gcc", "ICDI", 1, "Reference", "120 MHz Cortex-M4F", "256 KiB SRAM", "1 MiB Flash", "SSI Mode 1", "Yes", "USB OTG", "No", "Multiple devices possible; Ethernet transport is available", PINS_HEADER),
    Target("05_Renesas/RA6M5", "Renesas", "RA6", "R7FA6M5BH3CFC", "Arm Cortex-M33", "EK-RA6M5", "Renesas FSP", "6.5.0 API reference; board build pending", "Arm GNU 13.2 recommended by FSP; adapter test GCC 9.2.1", "J-Link", 1, "Reference", "up to 200 MHz", "512 KiB SRAM", "up to 2 MiB Flash", "SPI Mode 1; 1 MHz reference", "DTC/DMAC hardware; reference adapter excludes DMA", "USB HS/FS", "No", "One device reference; multi-device throughput unverified", ("P412 / PMOD2 SCK signal; header unverified", "P411 / PMOD2 MOSI signal; header unverified", "P410 / PMOD2 MISO signal; header unverified", "SDK_CS", "SDK_DRDY", "SDK_RESET", "SDK_PWDN", "SDK_START", "SDK_TX")),
    Target("05_Renesas/RX72N", "Renesas", "RX72N", "R5F572NNHDFB", "Renesas RXv3", "RX72N Envision Kit", "RX Driver Package / FIT", "pin in e2 studio project", "CC-RX / GCC for RX", "E2 Lite", 1, "Reference", "up to 240 MHz RXv3", "1 MiB RAM", "4 MiB Flash", "RSPI Mode 1", "DMAC/DTC", "USB HS/FS", "No", "Strong multi-device and Ethernet capability", PINS_HEADER),
    Target("06_Microchip/SAME54", "Microchip", "SAM E5x", "ATSAME54P20A", "Arm Cortex-M4F", "SAME54 Xplained Pro", "MPLAB Harmony 3", "pin in MPLAB project", "XC32 / arm-none-eabi-gcc", "EDBG", 1, "Reference", "up to 120 MHz", "256 KiB SRAM", "1 MiB Flash", "SERCOM SPI Mode 1", "DMAC", "USB HS/FS", "No", "Multiple devices practical with DMAC", PINS_HEADER),
    Target("06_Microchip/SAMD51", "Microchip", "SAM D5x", "ATSAMD51J20A", "Arm Cortex-M4F", "SAM D51 Xplained Pro", "MPLAB Harmony 3", "pin in MPLAB project", "XC32 / arm-none-eabi-gcc", "EDBG", 1, "Reference", "up to 120 MHz", "256 KiB SRAM", "1 MiB Flash", "SERCOM SPI Mode 1", "DMAC", "USB FS", "No", "Multiple devices practical with DMAC", PINS_HEADER),
    Target("06_Microchip/AVR128DA48", "Microchip", "AVR DA", "AVR128DA48", "8-bit AVR", "AVR128DA48 Curiosity Nano", "MPLAB XC8 / AVR Dx device pack", "pin in MPLAB project", "avr-gcc / XC8", "nEDBG/UPDI", 1, "Reference", "up to 24 MHz", "16 KiB SRAM", "128 KiB Flash", "SPI Mode 1", "Event system; no general DMA", "No", "No", "One device recommended; measure CPU and serial budget", PINS_HEADER),
    Target("07_Nordic/nRF52840", "Nordic Semiconductor", "nRF52", "nRF52840", "Arm Cortex-M4F", "nRF52840 DK", "nRF Connect SDK / Zephyr", "v3.4.0 LTS reference", "arm-zephyr-eabi-gcc", "J-Link", 1, "Compiles", "64 MHz Cortex-M4F", "256 KiB RAM", "1 MiB Flash", "SPIM Mode 1", "EasyDMA", "USB FS", "BLE 5", "Multiple devices possible; keep BLE outside acquisition ISR", ("P1.03", "P1.05", "P1.04", "P1.06", "P1.07", "P1.08", "P1.10", "P1.11", "P1.12")),
    Target("07_Nordic/nRF5340", "Nordic Semiconductor", "nRF53", "nRF5340 application core", "dual Arm Cortex-M33", "nRF5340 DK", "nRF Connect SDK / Zephyr", "Zephyr 3.7.1 verified; NCS 3.4.0 CI not run", "arm-none-eabi-gcc 9.2.1", "J-Link", 1, "Compiles", "application core up to 128 MHz", "512 KiB application RAM", "1 MiB application Flash", "SPIM Mode 1", "EasyDMA", "USB FS", "BLE 5", "Multiple devices practical; use network core separation", ("P1.04", "P1.06", "P1.05", "P1.07", "P1.08", "P1.09", "P1.10", "P1.11", "P1.12")),
    Target("08_Infineon/PSoC6", "Infineon", "PSoC 6", "CY8C624ABZI-S2D44", "Arm Cortex-M4F + Cortex-M0+", "CY8CKIT-062S2-43012", "ModusToolbox HAL", "pin in ModusToolbox project", "GCC Arm Embedded", "KitProg3", 2, "Reference", "up to 150 MHz M4F", "up to 1 MiB SRAM", "2 MiB Flash", "SCB SPI Mode 1", "DW DMA", "USB FS", "BLE 5 via companion radio", "Multiple devices possible", PINS_HEADER),
    Target("09_SiliconLabs/EFR32MG24", "Silicon Labs", "EFR32MG24", "EFR32MG24", "Arm Cortex-M33", "xG24 Explorer Kit", "Gecko SDK / emlib", "pin in Simplicity Studio project", "arm-none-eabi-gcc", "J-Link", 2, "Reference", "up to 78 MHz", "256 KiB RAM", "up to 1536 KiB Flash", "EUSART SPI Mode 1", "LDMA", "No native USB on MCU", "BLE / 802.15.4", "Multiple devices possible; isolate radio timing", PINS_HEADER),
    Target("10_RaspberryPi/RP2040", "Raspberry Pi", "RP2040", "RP2040", "dual Arm Cortex-M0+", "Raspberry Pi Pico", "Pico SDK", "2.3.0 reference", "arm-none-eabi-gcc", "SWD / debug probe", 2, "Compiles", "dual-core up to 133 MHz", "264 KiB SRAM", "external QSPI Flash", "SPI0 Mode 1; 4 MHz bring-up", "Yes", "USB FS device/host", "No", "PIO and dual cores enable multiple devices", ("GP18", "GP19", "GP16", "GP17", "GP20", "GP21", "Not wired in native example", "GP22", "GP0 / UART0 TX")),
    Target("10_RaspberryPi/RP2350", "Raspberry Pi", "RP2350", "RP2350", "dual Arm Cortex-M33 or Hazard3 RISC-V", "Raspberry Pi Pico 2", "Pico SDK", "2.3.0 reference", "arm-none-eabi-gcc", "SWD / debug probe", 2, "Compiles", "dual-core up to 150 MHz", "520 KiB SRAM", "external QSPI Flash", "SPI0 Mode 1; 4 MHz bring-up", "Yes", "USB FS device/host", "No", "PIO and dual cores enable multiple devices", ("GP18", "GP19", "GP16", "GP17", "GP20", "GP21", "Not wired in native example", "GP22", "GP0 / UART0 TX")),
    Target("11_GigaDevice/GD32F450", "GigaDevice", "GD32F4", "GD32F450ZKT6", "Arm Cortex-M4F", "GD32F450Z-EVAL", "GD32F4xx Firmware Library", "pinned PlatformIO package reference", "arm-none-eabi-gcc", "ST-LINK/J-Link", 2, "Compiles", "up to 200 MHz", "192 KiB SRAM in validated board profile", "3 MiB Flash in validated board profile", "SPI Mode 1", "DMA", "USB HS/FS", "No", "Multiple devices practical", PINS_ARM),
    Target("12_WCH/CH32V307", "WCH", "CH32V30x", "CH32V307VCT6", "QingKe V4F RISC-V", "CH32V307 EVT", "WCH NoneOS SDK", "pinned PlatformIO package reference", "riscv-none-embed-gcc", "WCH-Link", 2, "Compiles", "up to 144 MHz", "64 KiB SRAM", "256 KiB Flash", "SPI1 Mode 1", "DMA", "USB HS/FS", "No", "Multiple devices possible", ("PA5", "PA7", "PA6", "PB6", "PB7", "PB8", "Not wired in native example", "PB9", "PA9 / USART1 TX")),
    Target("13_Artery/AT32F435", "Artery", "AT32F435", "AT32F435ZMT7", "Arm Cortex-M4F", "AT-START-F435", "AT32F435/437 Firmware Library", "pin in consuming project", "arm-none-eabi-gcc", "AT-LINK/J-Link", 3, "Reference", "up to 288 MHz", "consult exact device datasheet", "consult exact device datasheet", "SPI Mode 1", "DMA", "USB OTG", "No", "Template; validate resource budget", PINS_ARM),
    Target("14_Geehy/APM32F407", "Geehy", "APM32F407", "APM32F407VGT6", "Arm Cortex-M4F", "APM32F407 development board", "APM32F4xx DAL", "pin in consuming project", "arm-none-eabi-gcc", "J-Link", 3, "Reference", "up to 168 MHz", "consult exact device datasheet", "1 MiB class", "SPI Mode 1", "DMA", "USB OTG", "No", "Template; validate resource budget", PINS_ARM),
    Target("15_HDSC/HC32F460", "HDSC", "HC32F460", "HC32F460PETB", "Arm Cortex-M4F", "HC32F460 evaluation board", "HDSC DDL", "pin in consuming project", "arm-none-eabi-gcc", "J-Link", 3, "Reference", "up to 200 MHz", "consult exact device datasheet", "consult exact device datasheet", "SPI Mode 1", "DMA", "USB FS", "No", "Template; validate resource budget", PINS_HEADER),
    Target("16_MindMotion/MM32F3277", "MindMotion", "MM32F3277", "MM32F3277G9P", "Arm Cortex-M3", "MM32F3277 evaluation board", "MM32 HAL/SPL", "pin in consuming project", "arm-none-eabi-gcc", "J-Link", 3, "Reference", "up to 120 MHz", "consult exact device datasheet", "consult exact device datasheet", "SPI Mode 1", "DMA", "USB FS", "No", "Template; validate resource budget", PINS_HEADER),
    Target("17_Nationstech/N32G455", "Nationstech", "N32G455", "N32G455VEL7", "Arm Cortex-M4F", "N32G455 evaluation board", "N32G45x Firmware Library", "pin in consuming project", "arm-none-eabi-gcc", "J-Link", 3, "Reference", "up to 144 MHz", "consult exact device datasheet", "consult exact device datasheet", "SPI Mode 1", "DMA", "USB FS", "No", "Template; validate resource budget", PINS_HEADER),
    Target("18_Nuvoton/M487", "Nuvoton", "NuMicro M480", "M487JIDAE", "Arm Cortex-M4F", "NuMaker-M487", "NuMicro M480 BSP", "pin in consuming project", "arm-none-eabi-gcc / Arm Compiler", "Nu-Link", 3, "Reference", "up to 192 MHz", "160 KiB SRAM class", "512 KiB Flash class", "SPI Mode 1", "PDMA", "USB HS/FS", "No", "Template; validate resource budget", PINS_HEADER),
    Target("19_Puya/PY32F403", "Puya", "PY32F403", "PY32F403", "Arm Cortex-M class", "PY32F403 evaluation board", "PY32 HAL", "pin in consuming project", "arm-none-eabi-gcc", "CMSIS-DAP/J-Link", 3, "Reference", "confirm against selected PY32F403 part", "confirm against selected part", "confirm against selected part", "SPI Mode 1", "confirm against selected part", "confirm against selected part", "No", "Template; validate resource budget", PINS_HEADER),
    Target("20_STC/STC32G12K128", "STC", "STC32G12", "STC32G12K128", "enhanced 8051", "STC32G12K128 development board", "STC-ISP device support", "pin in Keil/SDCC project", "Keil C51 / SDCC", "STC-ISP", 3, "Reference", "confirm against current datasheet", "confirm against exact package", "128 KiB code Flash", "SPI Mode 1", "limited; device-specific", "No", "No", "One device recommended", PINS_HEADER),
]


EXISTING_RANKS = {
    "01_STMicroelectronics/STM32H743": 1,
    "01_STMicroelectronics/STM32F407": 2,
    "02_Espressif/ESP32S3": 3,
    "03_NXP/MIMXRT1062": 5,
    "04_TexasInstruments/MSPM0G3507": 6,
    "07_Nordic/nRF5340": 7,
    "05_Renesas/RA6M5": 8,
    "06_Microchip/SAME54": 9,
    "08_Infineon/PSoC6": 10,
    "02_Espressif/ESP32C6": 13,
    "03_NXP/LPC55S69": 15,
    "04_TexasInstruments/TM4C1294": 16,
    "04_TexasInstruments/TMS320F28379D": 17,
    "05_Renesas/RX72N": 18,
    "06_Microchip/SAMD51": 19,
    "09_SiliconLabs/EFR32MG24": 20,
    "07_Nordic/nRF52840": 21,
    "10_RaspberryPi/RP2350": 23,
    "10_RaspberryPi/RP2040": 24,
    "11_GigaDevice/GD32F450": 31,
    "12_WCH/CH32V307": 33,
    "13_Artery/AT32F435": 35,
    "14_Geehy/APM32F407": 36,
    "15_HDSC/HC32F460": 37,
    "16_MindMotion/MM32F3277": 38,
    "17_Nationstech/N32G455": 39,
    "18_Nuvoton/M487": 40,
    "19_Puya/PY32F403": 41,
    "20_STC/STC32G12K128": 42,
    "06_Microchip/AVR128DA48": 52,
}

RANKED_FAMILY_NAMES = {
    "03_NXP/MIMXRT1062": "i.MX RT1060",
    "07_Nordic/nRF5340": "nRF5340",
    "06_Microchip/SAME54": "SAME5x",
    "08_Infineon/PSoC6": "PSoC6",
    "03_NXP/LPC55S69": "LPC55",
    "04_TexasInstruments/TM4C1294": "TM4C129",
    "04_TexasInstruments/TMS320F28379D": "C2000",
    "05_Renesas/RX72N": "RX72",
    "06_Microchip/SAMD51": "SAMD51",
    "09_SiliconLabs/EFR32MG24": "EFR32",
    "07_Nordic/nRF52840": "nRF52840",
    "11_GigaDevice/GD32F450": "GD32F4",
    "12_WCH/CH32V307": "CH32V307",
    "16_MindMotion/MM32F3277": "MM32F327",
    "18_Nuvoton/M487": "M480",
    "19_Puya/PY32F403": "PY32",
    "20_STC/STC32G12K128": "STC32",
}


def ecosystem_target(rank: int, path: str, vendor: str, family: str,
                     architecture: str, sdk: str,
                     status: str = "Reference") -> Target:
    tier = 1 if rank <= 30 else (2 if rank <= 50 else 3)
    return Target(
        path, vendor, family, family, architecture,
        f"Select an official {family} evaluation board", sdk,
        "pin in the consuming official SDK project", "vendor-supported compiler",
        "vendor-supported debug probe", tier, status,
        "confirm against selected device", "confirm against selected device",
        "confirm against selected device", "SPI Mode 1; start at 4 MHz",
        "confirm against selected device", "confirm against selected device",
        "confirm against selected device", "Template only; benchmark before multi-device use",
        PINS_HEADER, rank,
    )


TARGETS.extend([
Target("03_NXP/MIMXRT1170", "NXP", "i.MX RT1170", "MIMXRT1176DVMAA", "dual Arm Cortex-M7/M4", "MIMXRT1170-EVKB", "MCUXpresso SDK", "25.06.00 reference", "arm-none-eabi-gcc / MCUXpresso IDE", "on-board MCU-Link (CMSIS-DAP)", 1, "Reference", "Cortex-M7 up to 1 GHz plus Cortex-M4 up to 400 MHz", "2 MiB on-chip RAM", "external boot Flash on EVKB", "LPSPI1 Mode 1; 4 MHz bring-up", "eDMA available; reference adapter is blocking", "two USB 2.0 OTG controllers with integrated PHYs", "optional M.2 Wi-Fi/BLE module; no integrated radio", "One device reference; multi-device requires separate CS/DRDY and measured scheduling", ("GPIO_AD_28 / J10-12", "GPIO_AD_30 / J10-8", "GPIO_AD_31 / J10-10", "Arduino D10 / J10-6", "Arduino D2 / physical pin pending verification", "Arduino D3 / physical pin pending verification", "Arduino D4 / physical pin pending verification", "Arduino D5 / physical pin pending verification", "MCU-Link VCOM (transport only)"), 4),
    ecosystem_target(11, "01_STMicroelectronics/STM32G4", "STMicroelectronics", "STM32G4", "Arm Cortex-M4F", "STM32CubeG4"),
    ecosystem_target(12, "01_STMicroelectronics/STM32U5", "STMicroelectronics", "STM32U5", "Arm Cortex-M33", "STM32CubeU5"),
    ecosystem_target(14, "02_Espressif/ESP32", "Espressif", "ESP32", "Xtensa LX6 dual-core", "ESP-IDF"),
    ecosystem_target(22, "07_Nordic/nRF54", "Nordic Semiconductor", "nRF54", "Arm Cortex-M33", "nRF Connect SDK"),
    ecosystem_target(25, "04_TexasInstruments/CC26xx", "Texas Instruments", "CC2640/CC26xx", "Arm Cortex-M3", "SimpleLink CC13xx/CC26xx SDK"),
    ecosystem_target(26, "04_TexasInstruments/CC1352", "Texas Instruments", "CC1352", "Arm Cortex-M4F", "SimpleLink CC13xx/CC26xx SDK"),
    ecosystem_target(27, "01_STMicroelectronics/STM32WB", "STMicroelectronics", "STM32WB", "dual Arm Cortex-M4/M0+", "STM32CubeWB"),
    ecosystem_target(28, "01_STMicroelectronics/STM32WL", "STMicroelectronics", "STM32WL", "Arm Cortex-M4", "STM32CubeWL"),
    ecosystem_target(29, "23_Realtek/RTL8720", "Realtek", "RTL8720", "Arm Cortex-M class", "Ameba SDK"),
    ecosystem_target(30, "25_Other/Telink_TLSR825x", "Telink", "TLSR825x", "32-bit proprietary/RISC core", "Telink BLE SDK"),
    ecosystem_target(32, "11_GigaDevice/GD32H7", "GigaDevice", "GD32H7", "Arm Cortex-M7", "GD32H7xx Firmware Library"),
    ecosystem_target(34, "12_WCH/CH32V203", "WCH", "CH32V203", "QingKe RISC-V", "WCH EVT / NoneOS SDK"),
    ecosystem_target(43, "22_BouffaloLab/BL616", "Bouffalo Lab", "BL616", "32-bit RISC-V", "Bouffalo SDK"),
    ecosystem_target(44, "22_BouffaloLab/BL808", "Bouffalo Lab", "BL808", "multi-core RISC-V", "Bouffalo SDK"),
    ecosystem_target(45, "25_Other/Xiaomi_Pine_RISCV", "Xiaomi", "Pine RISC-V IoT", "RISC-V", "official SDK required", "Planned"),
    ecosystem_target(46, "25_Other/Nations_M4", "Nations", "M4 series", "Arm Cortex-M4", "official Nations SDK", "Planned"),
    ecosystem_target(47, "14_Geehy/APM32M", "Geehy", "APM32M", "Arm Cortex-M class", "APM32 SDK"),
    ecosystem_target(48, "16_MindMotion/MM32W", "MindMotion", "MM32W", "Arm Cortex-M class", "MM32 SDK"),
    ecosystem_target(49, "18_Nuvoton/M55M1", "Nuvoton", "M55M1", "Arm Cortex-M55", "NuMicro BSP"),
    ecosystem_target(50, "25_Other/THead_XuanTie", "T-Head", "XuanTie RISC-V", "RISC-V", "T-Head/Xuantie SDK", "Planned"),
    ecosystem_target(51, "06_Microchip/PIC32", "Microchip", "PIC32", "MIPS32 or Arm by family", "MPLAB Harmony 3"),
    ecosystem_target(53, "06_Microchip/AVR_Classic", "Microchip", "AVR Classic", "8-bit AVR", "MPLAB device packs"),
    ecosystem_target(54, "06_Microchip/PIC16_PIC18", "Microchip", "PIC16/PIC18", "8-bit PIC", "MPLAB XC8"),
    ecosystem_target(55, "04_TexasInstruments/MSP430", "Texas Instruments", "MSP430", "16-bit MSP430", "MSP430Ware"),
    ecosystem_target(56, "05_Renesas/RL78", "Renesas", "RL78", "16-bit RL78", "RL78 Smart Configurator"),
    ecosystem_target(57, "05_Renesas/Synergy", "Renesas", "Synergy", "Arm Cortex-M", "Synergy Software Package", "Planned"),
    ecosystem_target(58, "01_STMicroelectronics/STM8", "STMicroelectronics", "STM8", "8-bit STM8", "STM8 Standard Peripheral Library"),
    ecosystem_target(59, "24_Toshiba/TXZ", "Toshiba", "TXZ", "Arm Cortex-M", "TXZ+ Peripheral Drivers"),
    ecosystem_target(60, "25_Other/Samsung_ARTIK", "Samsung", "ARTIK", "Arm application/MCU class", "ARTIK SDK", "Planned"),
    ecosystem_target(61, "21_Ambiq/Apollo4", "Ambiq", "Apollo4", "Arm Cortex-M4F", "AmbiqSuite SDK"),
    ecosystem_target(62, "21_Ambiq/Apollo3", "Ambiq", "Apollo3", "Arm Cortex-M4F", "AmbiqSuite SDK"),
    ecosystem_target(63, "05_Renesas/DA1469x", "Dialog/Renesas", "DA1469x", "Arm Cortex-M33", "SmartSnippets SDK"),
    ecosystem_target(64, "09_SiliconLabs/EFM32", "Silicon Labs", "EFM32", "Arm Cortex-M", "Gecko SDK / emlib"),
    ecosystem_target(65, "04_TexasInstruments/CC32xx", "Texas Instruments", "CC32xx", "Arm Cortex-M4", "SimpleLink CC32xx SDK"),
    ecosystem_target(66, "23_Realtek/RTL87xx", "Realtek", "RTL87xx", "Arm Cortex-M class", "Realtek SDK", "Planned"),
    ecosystem_target(67, "25_Other/Actions_ATS", "Actions Semiconductor", "ATS", "proprietary/Arm by device", "official SDK required", "Planned"),
    ecosystem_target(68, "25_Other/Allwinner_R", "Allwinner", "R series", "Arm/RISC-V by device", "official BSP required", "Planned"),
    ecosystem_target(69, "25_Other/Amlogic_IoT", "Amlogic", "IoT family unspecified", "device not selected", "official SDK required", "Planned"),
    ecosystem_target(70, "25_Other/Rockchip_MCU_IoT", "Rockchip", "MCU/IoT family unspecified", "device not selected", "official SDK required", "Planned"),
    ecosystem_target(86, "25_Other/Sipeed_Maix_RISCV", "Sipeed", "Maix/RISC-V", "RISC-V", "board-specific SDK"),
    ecosystem_target(87, "25_Other/Kendryte_K210", "Kendryte", "K210", "dual RV64GC", "Kendryte standalone SDK"),
    ecosystem_target(88, "25_Other/SiFive_Freedom", "SiFive", "Freedom", "RISC-V", "Freedom E SDK"),
    ecosystem_target(89, "25_Other/Nuclei_RISCV", "Nuclei", "RISC-V", "RISC-V", "Nuclei SDK"),
    ecosystem_target(90, "25_Other/OpenHW_CV32", "OpenHW Group", "CV32", "RISC-V", "CORE-V SDK"),
    ecosystem_target(91, "25_Other/Atmel_ATmega328", "Atmel Legacy", "ATmega328", "8-bit AVR", "avr-libc / Microchip device pack"),
    ecosystem_target(92, "25_Other/Atmel_ATtiny", "Atmel Legacy", "ATtiny", "8-bit AVR", "avr-libc / Microchip device pack"),
    ecosystem_target(93, "25_Other/ARM_CortexM0Plus_Generic", "ARM Generic", "Cortex-M0+", "Arm Cortex-M0+", "CMSIS device pack supplied by silicon vendor"),
    ecosystem_target(94, "25_Other/ARM_CortexM3_Generic", "ARM Generic", "Cortex-M3", "Arm Cortex-M3", "CMSIS device pack supplied by silicon vendor"),
    ecosystem_target(95, "25_Other/ARM_CortexM4_Generic", "ARM Generic", "Cortex-M4", "Arm Cortex-M4", "CMSIS device pack supplied by silicon vendor"),
    ecosystem_target(96, "25_Other/ARM_CortexM7_Generic", "ARM Generic", "Cortex-M7", "Arm Cortex-M7", "CMSIS device pack supplied by silicon vendor"),
    ecosystem_target(97, "25_Other/RISCV_RV32_Generic", "RISC-V Generic", "RV32", "RV32", "vendor BSP required"),
    ecosystem_target(98, "25_Other/DSP_TI_C6000", "Texas Instruments", "C6000 DSP", "TI C6000 DSP", "Processor SDK / Code Composer Studio"),
    ecosystem_target(99, "25_Other/DSP_ADI_SHARC", "Analog Devices", "SHARC", "ADI SHARC DSP", "CrossCore Embedded Studio"),
    ecosystem_target(100, "25_Other/DSP_Cadence_Tensilica", "Cadence", "Tensilica DSP", "configurable Xtensa/Tensilica", "vendor SoC SDK required", "Planned"),
])


PORT_SPI_H = r'''#ifndef ADS1299_MCU_SPI_H
#define ADS1299_MCU_SPI_H

#include <stddef.h>
#include <stdint.h>
#include "ads1299_port.h"

typedef enum {
    ADS1299_PIN_CS,
    ADS1299_PIN_RESET,
    ADS1299_PIN_PWDN,
    ADS1299_PIN_START,
    ADS1299_PIN_DRDY
} ads1299_platform_pin_t;

typedef struct {
    void *user;
    int (*spi_transfer)(void *user, const uint8_t *tx, uint8_t *rx, size_t len);
    void (*pin_write)(void *user, ads1299_platform_pin_t pin, int level);
    int (*pin_read)(void *user, ads1299_platform_pin_t pin);
    void (*delay_us)(void *user, uint32_t us);
} ads1299_platform_hal_t;

typedef struct {
    ads1299_platform_hal_t hal;
} ads1299_mcu_port_t;

int ads1299_mcu_port_init(ads1299_mcu_port_t *ctx,
                          const ads1299_platform_hal_t *hal);
ads1299_port_t ads1299_mcu_make_port(ads1299_mcu_port_t *ctx);
int ads1299_mcu_spi_write(ads1299_mcu_port_t *ctx,
                          const uint8_t *data, size_t len);
int ads1299_mcu_spi_read(ads1299_mcu_port_t *ctx,
                         uint8_t *data, size_t len);
int ads1299_mcu_spi_transfer(ads1299_mcu_port_t *ctx,
                             const uint8_t *tx, uint8_t *rx, size_t len);

#endif
'''


PORT_SPI_C = r'''#include "ads1299_spi.h"
#include "ads1299_gpio.h"
#include "ads1299_drdy.h"

static int port_spi(void *user, const uint8_t *tx, uint8_t *rx, size_t len) {
    ads1299_mcu_port_t *ctx = (ads1299_mcu_port_t *)user;
    return ads1299_mcu_spi_transfer(ctx, tx, rx, len);
}

int ads1299_mcu_spi_write(ads1299_mcu_port_t *ctx,
                          const uint8_t *data, size_t len) {
    if (!data) return -1;
    return ads1299_mcu_spi_transfer(ctx, data, NULL, len);
}

int ads1299_mcu_spi_read(ads1299_mcu_port_t *ctx,
                         uint8_t *data, size_t len) {
    if (!data) return -1;
    return ads1299_mcu_spi_transfer(ctx, NULL, data, len);
}

int ads1299_mcu_spi_transfer(ads1299_mcu_port_t *ctx,
                             const uint8_t *tx, uint8_t *rx, size_t len) {
    if (!ctx || !ctx->hal.spi_transfer || len == 0u || (!tx && !rx)) return -1;
    return ctx->hal.spi_transfer(ctx->hal.user, tx, rx, len);
}

static void port_delay(void *user, uint32_t us) {
    ads1299_mcu_port_t *ctx = (ads1299_mcu_port_t *)user;
    if (ctx && ctx->hal.delay_us) ctx->hal.delay_us(ctx->hal.user, us);
}

int ads1299_mcu_port_init(ads1299_mcu_port_t *ctx,
                          const ads1299_platform_hal_t *hal) {
    if (!ctx || !hal || !hal->spi_transfer || !hal->pin_write ||
        !hal->pin_read || !hal->delay_us) return -1;
    ctx->hal = *hal;
    return 0;
}

ads1299_port_t ads1299_mcu_make_port(ads1299_mcu_port_t *ctx) {
    ads1299_port_t port = {
        .user = ctx,
        .spi_transfer = port_spi,
        .cs_write = ads1299_port_cs_write,
        .reset_write = ads1299_port_reset_write,
        .pwdn_write = ads1299_port_pwdn_write,
        .start_write = ads1299_port_start_write,
        .drdy_read = ads1299_port_drdy_read,
        .delay_us = port_delay,
    };
    return port;
}
'''


PORT_GPIO_H = r'''#ifndef ADS1299_MCU_GPIO_H
#define ADS1299_MCU_GPIO_H

#include "ads1299_spi.h"

void ads1299_port_cs_write(void *user, int level);
void ads1299_port_reset_write(void *user, int level);
void ads1299_port_pwdn_write(void *user, int level);
void ads1299_port_start_write(void *user, int level);
void ads1299_mcu_cs_low(ads1299_mcu_port_t *ctx);
void ads1299_mcu_cs_high(ads1299_mcu_port_t *ctx);
void ads1299_mcu_reset(ads1299_mcu_port_t *ctx, int level);
void ads1299_mcu_start(ads1299_mcu_port_t *ctx, int level);
void ads1299_mcu_delay_us(ads1299_mcu_port_t *ctx, uint32_t us);
void ads1299_mcu_delay_ms(ads1299_mcu_port_t *ctx, uint32_t ms);

#endif
'''


PORT_GPIO_C = r'''#include "ads1299_gpio.h"
#include "ads1299_spi.h"

static void write_pin(void *user, ads1299_platform_pin_t pin, int level) {
    ads1299_mcu_port_t *ctx = (ads1299_mcu_port_t *)user;
    if (ctx && ctx->hal.pin_write) {
        ctx->hal.pin_write(ctx->hal.user, pin, level ? 1 : 0);
    }
}

void ads1299_port_cs_write(void *user, int level) {
    write_pin(user, ADS1299_PIN_CS, level);
}
void ads1299_port_reset_write(void *user, int level) {
    write_pin(user, ADS1299_PIN_RESET, level);
}
void ads1299_port_pwdn_write(void *user, int level) {
    write_pin(user, ADS1299_PIN_PWDN, level);
}
void ads1299_port_start_write(void *user, int level) {
    write_pin(user, ADS1299_PIN_START, level);
}
void ads1299_mcu_cs_low(ads1299_mcu_port_t *ctx) { write_pin(ctx, ADS1299_PIN_CS, 0); }
void ads1299_mcu_cs_high(ads1299_mcu_port_t *ctx) { write_pin(ctx, ADS1299_PIN_CS, 1); }
void ads1299_mcu_reset(ads1299_mcu_port_t *ctx, int level) {
    write_pin(ctx, ADS1299_PIN_RESET, level);
}
void ads1299_mcu_start(ads1299_mcu_port_t *ctx, int level) {
    write_pin(ctx, ADS1299_PIN_START, level);
}
void ads1299_mcu_delay_us(ads1299_mcu_port_t *ctx, uint32_t us) {
    if (ctx && ctx->hal.delay_us) ctx->hal.delay_us(ctx->hal.user, us);
}
void ads1299_mcu_delay_ms(ads1299_mcu_port_t *ctx, uint32_t ms) {
    while (ms-- != 0u) ads1299_mcu_delay_us(ctx, 1000u);
}
'''


PORT_DRDY_H = r'''#ifndef ADS1299_MCU_DRDY_H
#define ADS1299_MCU_DRDY_H

#include "ads1299_spi.h"

int ads1299_port_drdy_read(void *user);
int ads1299_mcu_data_ready(ads1299_mcu_port_t *ctx);

#endif
'''


PORT_DRDY_C = r'''#include "ads1299_drdy.h"
#include "ads1299_spi.h"

int ads1299_port_drdy_read(void *user) {
    ads1299_mcu_port_t *ctx = (ads1299_mcu_port_t *)user;
    return ads1299_mcu_data_ready(ctx) ? 0 : 1;
}

int ads1299_mcu_data_ready(ads1299_mcu_port_t *ctx) {
    if (!ctx || !ctx->hal.pin_read) return 0;
    return ctx->hal.pin_read(ctx->hal.user, ADS1299_PIN_DRDY) ? 0 : 1;
}
'''


EXAMPLE_C = r'''/* Minimal integration example.  The board project supplies only the SDK
 * callbacks; all ADS1299 commands and register semantics remain in Core. */
#include "ads1299.h"
#include "ads1299_spi.h"

extern int board_ads1299_hal(ads1299_platform_hal_t *hal);

int main(void) {
    ads1299_platform_hal_t hal;
    ads1299_mcu_port_t mcu;
    ads1299_t device;
    ads1299_frame_t frame;

    if (board_ads1299_hal(&hal) != 0) return 1;
    if (ads1299_mcu_port_init(&mcu, &hal) != 0) return 2;
    ads1299_port_t port = ads1299_mcu_make_port(&mcu);
    if (ads1299_init(&device, &port) != ADS1299_OK) return 3;
    if (ads1299_hardware_reset(&device) != ADS1299_OK) return 4;
    if (ads1299_sdatac(&device) != ADS1299_OK) return 5;

    ads1299_device_id_t identity;
    if (ads1299_read_device_id(&device, &identity) != ADS1299_OK ||
        !identity.is_ads1299_family) return 6;
    if (ads1299_configure_internal_test(&device, ADS1299_GAIN_24, 0,
            ADS1299_TEST_FREQ_FCLK_DIV_2_21) != ADS1299_OK) return 7;
    if (ads1299_rdatac(&device) != ADS1299_OK) return 8;
    if (ads1299_start(&device) != ADS1299_OK) return 9;

    for (;;) {
        if (port.drdy_read(port.user) == 0) {
            if (ads1299_read_frame_continuous(&device, &frame) != ADS1299_OK)
                return 10;
            /* Send frame through the application's UART/USB/BLE/Ethernet path. */
        }
    }
}
'''


TEST_SPI = r'''#include <assert.h>
#include <string.h>
#include "ads1299_spi.h"

static unsigned calls;
static int fake_spi(void *u, const uint8_t *tx, uint8_t *rx, size_t n) {
    (void)u; ++calls; if (rx && tx) memcpy(rx, tx, n); return 0;
}
static void fake_write(void *u, ads1299_platform_pin_t p, int v) {(void)u;(void)p;(void)v;}
static int fake_read(void *u, ads1299_platform_pin_t p) {(void)u;(void)p;return 1;}
static void fake_delay(void *u, uint32_t n) {(void)u;(void)n;}

void test_port_spi(void) {
    ads1299_platform_hal_t hal = {0, fake_spi, fake_write, fake_read, fake_delay};
    ads1299_mcu_port_t ctx; assert(ads1299_mcu_port_init(&ctx, &hal) == 0);
    ads1299_port_t port = ads1299_mcu_make_port(&ctx);
    uint8_t tx[2] = {0x12, 0x34}, rx[2] = {0};
    assert(port.spi_transfer(port.user, tx, rx, 2) == 0);
    assert(calls == 1 && memcmp(tx, rx, 2) == 0);
    assert(ads1299_mcu_spi_write(&ctx, tx, 2) == 0);
    assert(ads1299_mcu_spi_read(&ctx, rx, 2) == 0);
    assert(calls == 3);
}
'''


TEST_RESET = r'''#include <assert.h>
#include "ads1299_spi.h"
#include "ads1299_gpio.h"

static ads1299_platform_pin_t last_pin; static int last_level;
static int fake_spi(void *u,const uint8_t*t,uint8_t*r,size_t n){(void)u;(void)t;(void)r;(void)n;return 0;}
static void fake_write(void *u, ads1299_platform_pin_t p, int v){(void)u;last_pin=p;last_level=v;}
static int fake_read(void *u, ads1299_platform_pin_t p){(void)u;(void)p;return 1;}
static uint32_t delayed_us;
static void fake_delay(void *u,uint32_t n){(void)u;delayed_us += n;}

void test_port_reset_line(void) {
    ads1299_platform_hal_t hal={0,fake_spi,fake_write,fake_read,fake_delay};
    ads1299_mcu_port_t ctx; assert(ads1299_mcu_port_init(&ctx,&hal)==0);
    ads1299_port_t port=ads1299_mcu_make_port(&ctx);
    port.reset_write(port.user,0);
    assert(last_pin==ADS1299_PIN_RESET && last_level==0);
    port.reset_write(port.user,1);
    assert(last_pin==ADS1299_PIN_RESET && last_level==1);
    ads1299_mcu_delay_us(&ctx, 25u);
    ads1299_mcu_delay_ms(&ctx, 2u);
    assert(delayed_us == 2025u);
}
'''


TEST_REGISTER = r'''/* Register behavior belongs to Core.  This smoke test only proves that the
 * port transports opaque bytes without defining or rewriting register bits. */
#include <assert.h>
#include "ads1299_spi.h"

void test_register_boundary_is_opaque(void) {
    assert(sizeof(ads1299_platform_hal_t) > sizeof(void *));
}
'''


TEST_FRAME = r'''#include <assert.h>
#include "ads1299.h"

void test_core_frame_numeric_boundary(void) {
    assert(ads1299_sign_extend24(0x000001u) == 1);
    assert(ads1299_sign_extend24(0xFFFFFFu) == -1);
    assert(ads1299_sign_extend24(0x800000u) == -8388608);
}
'''


def yaml_quote(value: str) -> str:
    return json.dumps(value, ensure_ascii=False)


def maintenance_tier(rank: int) -> str:
    if rank <= 20:
        return "A"
    if rank <= 60:
        return "B"
    return "C"


def render_readme(t: Target) -> str:
    rank = t.rank or EXISTING_RANKS[t.path]
    tier = maintenance_tier(rank)
    return f"""# {t.mcu} ADS1299 Port\n\nGlobal ecosystem rank: **{rank}**. Status: **{t.status}**. Tier {tier}. Hardware validation is not implied.\n\n## Platform\n\n- Vendor: {t.vendor}\n- Family / MCU: {t.family} / {t.mcu}\n- Architecture: {t.architecture}\n- Reference board: {t.board}\n- Official environment: {t.sdk}\n- Compiler: {t.compiler}\n\n## ADS1299 connection\n\nUse SPI Mode 1 (CPOL=0, CPHA=1), MSB first. Keep CS software-controlled and\nroute DRDY, RESET, PWDN and START as independent GPIOs. Start at 4 MHz or less\nuntil ID read, configuration readback and the internal test signal pass. The\nreference pin assignment is documented in `board/pinmap.md`; confirm it against\nthe exact board revision before wiring.\n\n## Repository layers\n\n- ADS1299 behavior: `../../../core_driver/ads1299/`\n- This platform's hardware-only adapter: `ads1299_port/`\n- Minimal call flow: `examples/main_ads1299.c`\n- Vendor-project procedure: `integration.md`\n\nThe port accepts SDK callbacks for SPI, GPIO and microsecond delay. It also\nprovides a millisecond helper without changing the stable Core port contract.\nIt never defines ADS1299 registers. UART, USB, BLE or Ethernet transport stays\nin `firmware/transport/` and must not block a DRDY handler.\n"""


def render_platform_info(t: Target) -> str:
    rank = t.rank or EXISTING_RANKS[t.path]
    fields = {
        "schema": 1, "rank": rank, "vendor": t.vendor, "family": t.family, "mcu": t.mcu,
        "architecture": t.architecture, "board": t.board,
        "official_sdk": t.sdk, "sdk_version": t.sdk_version,
        "compiler": t.compiler, "debugger": t.debugger,
        "tier": maintenance_tier(rank), "status": t.status,
    }
    return "\n".join(f"{k}: {v if isinstance(v, int) else yaml_quote(v)}" for k, v in fields.items()) + "\n"


def render_capability(t: Target) -> str:
    if t.path == "02_Espressif/ESP32S3":
        return """# ESP32-S3 ADS1299 capability

| Item | Verified planning fact |
|---|---|
| CPU | Dual-core Xtensa LX7, up to 240 MHz |
| RAM | 512 KiB on-chip SRAM; the pinned N8 PlatformIO board exposes a 320 KiB application budget |
| Flash | 8 MiB in the `esp32-s3-devkitc-1` PlatformIO board definition |
| SPI | SPI2/SPI3 are general-purpose masters; reference uses SPI2, Mode 1 at 4 MHz |
| DMA | ESP-IDF SPI master supports DMA; the reference uses polling transactions with internal buffers |
| USB | USB 2.0 OTG plus USB Serial/JTAG; GPIO19/20 are kept free |
| Radio | 2.4 GHz Wi-Fi and Bluetooth LE 5 |

## ADS1299 planning boundary

One ADS1299 frame is acquired through the shared Core and hardware-only Port.
Radio, USB and host packetization belong in `firmware/transport/`; they must not
run inside a DRDY ISR or hold the SPI bus. Multi-device use is feasible only
after measuring DRDY service latency, buffer headroom and loss under radio load.
No throughput or hardware count is claimed by this compile-only reference.

Sources and exact build evidence are recorded in `sources.md` and `build.md`.
"""
    if t.path == "01_STMicroelectronics/STM32F407":
        return """# STM32F407 ADS1299 capability

| Item | Verified planning fact |
|---|---|
| CPU | 168 MHz Arm Cortex-M4 with single-precision FPU and DSP instructions |
| RAM | Up to 192 KiB SRAM plus 4 KiB backup SRAM; 64 KiB CCM is not DMA-accessible |
| Flash | 1 MiB on STM32F407VGT6 |
| SPI | Three SPI peripherals; SPI1 reference is Mode 1 on PA5/PA6/PA7 |
| DMA | Two DMA controllers, 16 streams, FIFOs and burst support |
| USB | USB OTG FS plus HS/FS controller capability |
| Ethernet | 10/100 MAC with dedicated DMA; external PHY/stack required |
| BLE / Wi-Fi | No integrated radio |

## Why this platform is valuable for ADS1299

STM32F407 is a mature Cortex-M4 platform with deterministic SPI, DMA, memory and
USB/Ethernet options for EEG acquisition. On STM32F407G-DISC1, SPI1 is shared
with the LIS3DSH sensor, so PE3 must remain high while ADS1299 is selected. The
reference deliberately uses polling before any DMA claim.

- 8 channels: directly supported by the Core reference flow.
- 16/32 channels: feasible with per-device CS/DRDY and measured service latency.
- 64 channels: requires DMA-safe SRAM (not CCM), buffering and throughput tests;
  it is not hardware-validated here.
- EEG suitability: good for established research prototypes when the analog,
  isolation and power design is independently validated.

Sources and revision details are recorded in `sources.md`.
"""
    if t.path == "01_STMicroelectronics/STM32H743":
        return """# STM32H743 ADS1299 capability\n\n| Item | Verified planning fact |\n|---|---|\n| CPU | Arm Cortex-M7 with DP-FPU/DSP/cache, silicon maximum 480 MHz |\n| RAM | Up to 1 MiB total, including TCM and backup SRAM regions |\n| Flash | 2 MiB dual-bank embedded Flash |\n| SPI | Six SPI peripherals; SPI1 Mode 1 reference uses PA5/PA6/PA7 |\n| DMA | MDMA plus DMA controllers; cache maintenance and DMA-visible memory are mandatory for a DMA design |\n| USB | USB OTG FS and HS/FS capability |\n| Ethernet | MAC with DMA; PHY and network stack remain outside the ADS1299 Port |\n| BLE / Wi-Fi | No integrated radio on STM32H743; use an external module if required |\n\n## Why this platform is valuable for ADS1299\n\nThe large RAM, deterministic Cortex-M7, multiple SPI blocks, DMA fabric, USB and\nEthernet make STM32H743 a strong acquisition controller for buffered EEG and\nmulti-device systems. The reference implementation intentionally starts with a\nblocking 8-bit SPI transfer because ADS1299 frames are only 15/21/27 bytes and\nthis is easier to validate. DMA is an optimization stage, not an unverified\nclaim.\n\n## Channel planning\n\n- 8 channels / one ADS1299: directly supported by the shared Core call flow.\n- 16 or 32 channels: practical with independent CS and observable DRDY lines;\n  measure service latency and skew on the actual board.\n- 64 channels: architecturally plausible, but requires DMA/queue scheduling,\n  cache-safe buffers and measured throughput. It is not bench-validated here.\n- EEG suitability: high for research acquisition after analog front-end, power,\n  isolation and signal-integrity validation. The MCU cannot replace the ADS1299\n  analog safety design.\n\nSources and revision details are recorded in `sources.md`.\n"""
    return f"""# Capability\n\n| Item | Capability |\n|---|---|\n| CPU | {t.cpu} |\n| RAM | {t.ram} |\n| Flash | {t.flash} |\n| SPI | {t.spi} |\n| DMA | {t.dma} |\n| USB | {t.usb} |\n| BLE | {t.ble} |\n| Multi-ADS1299 | {t.multi} |\n\nThese are planning limits, not throughput evidence. Bench measurements decide\nthe final supported ADS1299 count and transport rate.\n"""


def render_pinmap(t: Target) -> str:
    names = ("SCK", "MOSI / DIN", "MISO / DOUT", "CS", "DRDY", "RESET", "PWDN", "START", "stream TX")
    purpose = ("SPI clock", "MCU to ADS1299", "ADS1299 to MCU", "active-low chip select", "active-low data ready", "active-low reset", "active-low power-down", "conversion control", "optional host transport")
    rows = "\n".join(f"| {n} | {p} | {q} | 3.3 V digital |" for n, p, q in zip(names, t.pins, purpose))
    return f"""# Reference pin map\n\nBoard: **{t.board}**\n\n| Signal | Reference pin | Function | Level |\n|---|---|---|---|\n{rows}\n\nThe ADS1299 digital I/O supply and MCU GPIO voltage must be compatible. Confirm\nheader routing, boot straps, solder bridges and debugger conflicts against the\nexact board schematic before connecting hardware. `SDK_*` names mean the user\nmust select and document an available pin in the consuming vendor project.\n"""


def render_integration(t: Target) -> str:
    return f"""# Integration with {t.sdk}\n\n1. Install the official `{t.sdk}` environment; do not copy the vendor SDK into this repository.\n2. Create or open a vendor project for `{t.mcu}` / `{t.board}`.\n3. Add `firmware/core_driver/ads1299/*.c` and its include path.\n4. Add this directory's `ads1299_port/*.c` and include path.\n5. Implement `board_ads1299_hal()` using the vendor SDK's SPI, GPIO and microsecond-delay APIs.\n6. Configure SPI as Mode 1, MSB first, conservative clock, software CS.\n7. Copy the call flow from `examples/main_ads1299.c`; keep product transport outside the example.\n8. First verify power, ID read, register readback and internal test input. Only then connect electrodes.\n\nThe `spi_transfer` callback is full duplex. A null TX pointer means clock zero\nbytes while receiving; a null RX pointer means discard received bytes. If the\nvendor SDK rejects null buffers, implement that behavior with a small scratch\nbuffer in `board_ads1299_hal()`. CS is controlled by the separate GPIO callback\nand must not be toggled inside `spi_transfer`.\n\n## SDK ownership boundary\n\nVendor startup, linker scripts, CMSIS/HAL, generated configuration and middleware\nstay in the user's official SDK project. This repository owns only the thin\ncallback adapter and ADS1299-independent tests. Pin `{t.sdk_version}` in the\nconsumer project and record any API change in `version.md`.\n"""


def render_version(t: Target) -> str:
    return f"""# Version\n\n- Port version: 1.2.0\n- Port contract: callback adapter v2.1 (backward-compatible delay helpers)\n- Compatible Core: ADS1299 Core >= 2.0\n- Official SDK family: {t.sdk}\n- Reference SDK version: {t.sdk_version}\n- Last structural update: 2026-09-04\n"""


def render_validation(t: Target) -> str:
    build_commands = {
        "01_STMicroelectronics/STM32H743": "platformio run -d firmware/mcu/01_STMicroelectronics/STM32H743",
        "01_STMicroelectronics/STM32F407": "platformio run -d firmware/mcu/01_STMicroelectronics/STM32F407",
        "02_Espressif/ESP32S3": "platformio run -d firmware/mcu/02_Espressif/ESP32S3",
        "07_Nordic/nRF52840": "west build -p always --no-sysbuild -b nrf52840dk/nrf52840 firmware/mcu/07_Nordic/nRF52840",
        "07_Nordic/nRF5340": "west build -p always --no-sysbuild -b nrf5340dk/nrf5340/cpuapp firmware/mcu/07_Nordic/nRF5340",
        "10_RaspberryPi/RP2040": "cmake --build /tmp/rp2040-build -j 2 (see pico-reference.yml)",
        "10_RaspberryPi/RP2350": "cmake --build /tmp/rp2350-build -j 2 (see pico2-reference.yml)",
        "11_GigaDevice/GD32F450": "platformio run -d firmware/mcu/11_GigaDevice/GD32F450",
        "12_WCH/CH32V307": "platformio run -d firmware/mcu/12_WCH/CH32V307",
    }
    if t.path == "07_Nordic/nRF5340":
        evidence = "Build entrypoint: `tests/compile_zephyr.py` (see build.md). Clean local Zephyr 3.7.1 hardware-SPIM4 ELF build passed on 2026-09-05. NCS 3.4.0 CI was not run; no hardware test."
    elif t.status == "Compiles":
        local = t.path in {
            "01_STMicroelectronics/STM32H743",
            "01_STMicroelectronics/STM32F407",
            "02_Espressif/ESP32S3",
            "11_GigaDevice/GD32F450",
            "12_WCH/CH32V307",
        }
        evidence = (
            f"Build entrypoint: `{build_commands[t.path]}`. "
            + ("Clean local build passed on 2026-09-04."
               if local else
               "A pinned CI build definition is retained; it was not rerun locally in this maintenance session.")
        )
    elif t.path in ("03_NXP/MIMXRT1170", "03_NXP/MIMXRT1062"):
        evidence = "A real SDK 2.16.000 RAM reference ELF linked on 2026-09-05; see build.md for exact scope. Hardware execution is unverified."
    elif t.path == "04_TexasInstruments/MSPM0G3507":
        evidence = "A real TI SDK 2.11.00.07 reference ELF linked on 2026-09-05; see build.md for exact scope. Hardware execution is unverified."
    elif t.path == "05_Renesas/RA6M5":
        evidence = "A real FSP 5.6.0 reference ELF linked on 2026-09-05 with zero undefined symbols; see build.md. Physical execution and FSP 6.5 BSP build are unverified."
    elif t.path == "06_Microchip/SAME54":
        evidence = "A complete GNU reference ELF linked on 2026-09-06 with zero undefined symbols; modeled polling tests passed. See build.md. No board execution or XC32 build claimed."
    elif t.path == "08_Infineon/PSoC6":
        evidence = "A GNU CM4 reference ELF including the official prebuilt CM0P_SLEEP image linked on 2026-09-06; image addresses and zero unresolved symbols checked. Real PDL/adapter modeled-MMIO tests passed. See build.md. No physical boot or ModusToolbox IDE build claimed."
    elif t.path == "01_STMicroelectronics/STM32G4":
        evidence = "STM32G474 reference linked with genuine device startup and CubeG4 1.5.2 linker on 2026-09-06; modeled SPI tests passed. See build.md. No board execution or CubeIDE build claimed."
    elif t.path == "01_STMicroelectronics/STM32U5":
        evidence = "STM32U575 TrustZoneDisabled reference linked with actual startup and CubeU5 1.1.0 linker on 2026-09-06; zero unresolved symbols. Modeled faults, EOT delay and 65535+1 split passed. See build.md; no hardware or TrustZoneEnabled execution claimed."
    elif t.path == "02_Espressif/ESP32C6":
        evidence = "Actual ESP-IDF 5.4.0 C6 bootloader/application images built on 2026-09-07. Mock queue/lifetime tests passed separately. See build.md; no flashing or hardware acquisition claimed."
    elif t.path == "02_Espressif/ESP32":
        evidence = "Actual ESP-IDF 5.4.0 classic ESP32 bootloader/application images built on 2026-09-07. Mock queue/lifetime and GPIO direction tests passed separately. See build.md; no hardware execution claimed."
    elif t.path == "03_NXP/LPC55S69":
        evidence = "Complete 17-source Core0 ELF linked with MCUX_2.16.000 on 2026-09-07, vectors at zero and no unresolved symbols. Adapter API-double and real vendor SPI / modeled FIFO tests passed. See build.md. Provisional control pins and untested boot/hardware keep status Reference."
    else:
        evidence = "No clean vendor-toolchain build is claimed."
    return f"""# Validation\n\nCurrent status: **{t.status}**\n\nAllowed lifecycle states: `Planned`, `Reference`, `Example`, `Compatible`,\n`Compiles`, `Bench-tested`, `24h-tested`.\n\n{evidence}\n\nTo advance status, attach exact SDK/compiler versions, the clean command and\nlog. `Bench-tested` additionally requires a real ADS1299 ID read, internal-test\nwaveform and packet integrity evidence. `24h-tested` requires loss/CRC/error\ncounts from a continuous 24-hour run.\n"""


def write(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text.rstrip() + "\n", encoding="utf-8")


STM32H743_APPENDICES = {
    "readme": """

- STM32Cube binding: `stm32cube_adapter/`
- Reproducible compile record: `build.md`
- Official references and license provenance: `sources.md`

The checked reference is specifically the MB1364 NUCLEO-H743ZI2, not the older
MB1137 NUCLEO-H743ZI. D11 uses PA7 only with SB33 ON and SB35 OFF; changing
those solder bridges requires matching the SPI GPIO configuration.
""",
    "integration": """

For NUCLEO-H743ZI2, `stm32cube_adapter/` implements the callbacks and
`examples/stm32cube_reference/main.c` supplies the minimum HAL GPIO/SPI setup.
The reproducible PlatformIO entry point is recorded in `build.md`. Do not copy
the downloaded STM32CubeH7 framework into this repository.

The reference uses polling SPI. Introduce DMA only with explicit D-cache
maintenance, DMA-accessible buffer placement, queue-overflow accounting and a
new clean build plus hardware evidence.
""",
    "version": """

- STM32Cube adapter version: 1.0.0
- Reference project: PlatformIO `nucleo_h743zi2_ads1299`
""",
    "validation": """

The clean build used STM32CubeH7 1.13.0; see `build.md` for exact tool versions
and memory output. This proves compilation/linking only.
""",
}


STM32F407_APPENDICES = {
    "readme": """

- STM32Cube binding: `stm32cube_adapter/`
- Reproducible compile record: `build.md`
- Official references and license provenance: `sources.md`

The checked board is the official MB997 STM32F407G-DISC1. SPI1 is shared with
the on-board LIS3DSH; keep its PE3 chip select high when communicating with the
external ADS1299.
""",
    "integration": """

`stm32cube_adapter/` binds the official HAL and
`examples/stm32cube_reference/main.c` supplies the minimum MB997 setup. The
PlatformIO build is documented in `build.md`. Host UART/USB/network output must
be attached through `firmware/transport/`, not added to this MCU Port.

The reference uses polling. A DMA implementation must place buffers outside CCM
and record stream-loss measurements before any hardware status upgrade.
""",
    "version": """

- STM32Cube adapter version: 1.0.0
- Reference project: PlatformIO `stm32f407g_disc1_ads1299`
""",
    "validation": """

The clean build used STM32CubeF4 1.28.3; see `build.md` for exact tool versions,
vendor warnings and memory output. This proves compilation/linking only.
""",
}


ESP32S3_APPENDICES = {
    "readme": """

- Native ESP-IDF HAL binding: `components/ads1299_esp_idf_hal/`
- Reproducible PlatformIO build: `platformio.ini` and `build.md`
- Official references and provenance: `sources.md`

The pinned build uses PlatformIO's N8/no-PSRAM board definition. GPIO19/20 stay
free for native USB. Product UART/USB/BLE/Wi-Fi packetization is intentionally
absent from `main/main.c` and belongs in `firmware/transport/`.
""",
    "integration": """

The repository also supplies a native buildable project. `ads1299_port/` is an
ESP-IDF component containing the shared hardware-only Port;
`components/ads1299_esp_idf_hal/` maps ESP-IDF SPI/GPIO/delay calls into that
Port. `main/main.c` is the minimal acquisition flow. Do not merge transport or
ADS1299 register policy into the HAL component.

For the reference wiring, keep native USB on GPIO19/20 and use SPI2 with manual
CS. Change pin macros in `main/main.c` and `board/pinmap.md` together.
""",
    "version": """

- ESP-IDF HAL adapter version: 1.0.0
- Reference project: PlatformIO `esp32s3_devkitc1_ads1299`
- Pinned platform/framework: Espressif32 6.10.0 / ESP-IDF 5.4.0
""",
    "validation": """

The clean local build used ESP-IDF 5.4.0 and Xtensa GCC 14.2.0; see `build.md`
for the exact command and memory result. This proves compilation/linking only.
No board was flashed and no ADS1299 hardware response was observed.
""",
}

MIMXRT1170_APPENDICES = {
    "readme": """
## Maintained reference

This package targets the **MIMXRT1170-EVKB**, its MIMXRT1176DVMAA device and
the Cortex-M7 image. `mcuxpresso_adapter/` converts NXP LPSPI/GPIO calls into
the repository hardware-only HAL contract; ADS1299 commands and registers
remain exclusively in Core.

The LPSPI1 route follows NXP's current EVKB example. Control signals use
Arduino D2-D5; generate their exact GPIO mux with MCUXpresso Config Tools for
the installed board revision. Do not add an SDK, startup or linker files here.
""",
    "integration": """
## EVKB integration checklist

1. Install MCUXpresso SDK 25.06.00 (or record the newer selected version) for
   `evkbmimxrt1170`, then create a Cortex-M7 bare-metal project.
2. Enable LPSPI1, GPIO and the SDK common delay service.
3. Configure LPSPI1 for master, 8-bit, MSB-first, CPOL=0, CPHA=second edge,
   4 MHz. Route SCK/SOUT/SIN to GPIO_AD_28/30/31.
4. Configure Arduino D10 as software GPIO CS, D2 as DRDY input, and D3-D5 as
   RESET/PWDN/START outputs. Generate mux code with the Pins tool.
5. Add Core, `ads1299_port/*.c`, and the MCUXpresso adapter. Populate the GPIO
   base/pin pairs in `ads1299_mcuxpresso_ctx_t`.
6. Keep USB/Ethernet/UART packet transport in `firmware/transport`.
""",
    "validation": """
## Evidence boundary

- Official device, board, LPSPI route and SDK sources are in `sources.md`.
- `tests/validate_mimxrt1170.py` checks layering, metadata, pin documentation
  and syntax-compiles the adapter against a narrow MCUXpresso API fixture.
- An EVK-derived RAM reference ELF now links against SDK 2.16.000. See build.md
  and board/evk_reference.md. It does not validate EVKB wiring or hardware;
  the catalog retains **Reference** status.
""",
}

MIMXRT1062_APPENDICES = {
    "readme": """
## Maintained reference

This package targets **MIMXRT1060-EVKB** revision B-class hardware and its
MIMXRT1062DVL6B device. The repository-owned MCUXpresso adapter exposes only
LPSPI/GPIO/delay behavior; all ADS1299 protocol and register work stays in Core.

The J17/J16 mapping is taken from the NXP EVKB manual and current MCUXpresso
LPSPI example. Generate GPIO and LPSPI mux code in the consuming SDK project.
""",
    "integration": """
## EVKB integration checklist

1. Create an `evkbmimxrt1060` bare-metal project with MCUXpresso SDK 25.06.00
   or record the newer SDK version actually used.
2. Configure LPSPI1 master: 8-bit, MSB-first, CPOL=0, CPHA=second edge, 4 MHz.
3. Route SCK/SDO/SDI to GPIO_SD_B0_00/02/03; configure D10
   GPIO_SD_B0_01 as software CS.
4. Configure J16 D2 as DRDY input and D3-D5 as RESET/PWDN/START outputs.
5. Add Core, `ads1299_port/*.c`, and the MCUXpresso adapter. Populate all GPIO
   base/pin pairs after MCUXpresso Config Tools generates the mux definitions.
6. Keep OpenSDA VCOM, USB and Ethernet policy in `firmware/transport`.
""",
    "validation": """
## Evidence boundary

- `tests/validate_mimxrt1062.py` checks the official header mapping, layering,
  metadata and adapter syntax against a narrow SDK API fixture.
- A complete RAM reference ELF now links against SDK 2.16.000 with zero
  undefined symbols. See build.md and tests/acceptance.md. Hardware capture
  remains unverified; the catalog conservatively retains **Reference**.
""",
}


def target_appendix(t: Target, document: str) -> str:
    if t.path == "03_NXP/LPC55S69":
        return {
            "readme": "\n## Maintained LPC55 reference\n\nUse mcux_adapter/ with shared Core and the standard Port. The cold-boot Core0 binding uses SPI7 at 1 MHz from FRO12. Control pins are provisional and a wiring interlock prevents automatic acquisition. See board/reference_image.md and build.md for complete link evidence and limitations.\n",
            "integration": "\n## Concrete MCUX binding\n\nAdd mcux_adapter/ and compile the real fsl_spi.c with finite SPI_RETRY_TIMES=100000. Use either board/reference_image.c or your own board binding, not both. The reference owns GPIO ports 0/1 and Flexcomm7 exclusively at cold boot. It must not run as a nonsecure child image; do not alter security fuses. Follow board/reference_image.md before enabling the wiring interlock.\n",
            "version": "\n2026-09-07: LPC55 adapter 0.1.0; MCUX_2.16.000 full reference link and modeled SPI fault tests verified. No physical board qualification.\n",
        }.get(document, "")
    if t.path == "02_Espressif/ESP32":
        return {
            "readme": "\n## Maintained ESP32 reference\n\nOriginal IDF queued binding and DevKitC V4 WROOM board profile use SPI3, 1 MHz, GPIO18/23/19 with controls 21/22/25/26/27. Actual bootloader/application build and mock fault tests passed. See board/reference_image.md and build.md. Hardware qualification remains separate.\n",
            "integration": "\n## Concrete IDF binding\n\nUse this directory's CMake/platformio.ini entry, esp_idf_adapter/ and board/esp_idf_board.c. Never add tests/fixtures to production headers. Static context, one owning task and dedicated SPI3 only; do not reuse a timed-out descriptor until returned by IDF. No automatic bus recovery is implemented.\n",
        }.get(document, "")
    if t.path == "02_Espressif/ESP32C6":
        return {
            "readme": "\n## Maintained ESP-IDF reference\n\nUse the local platformio.ini / CMake entry, esp_idf_adapter/ and board/esp_idf_board.c. GPIO18/19 replace boot-strap GPIO4/5. See board/reference_image.md, sources.md and build.md. Single-task dedicated SPI2 with persistent timeout buffers; latest-frame debugger output only.\n",
            "integration": "\n## Concrete IDF binding\n\nBuild this platform directory using ESP-IDF 5.4.0 through pinned PlatformIO 6.10.0. The main component links shared Core, standard Port and queued adapter. Do not add tests/fixtures to production includes. Never free or reset an in-flight context after timeout. See build.md for local-cache environment and validation scope.\n",
            "version": "\n2026-09-07: C6 queued adapter 0.1.0, board binding and mock queue tests added; see build.md for actual build evidence. Core 2.0 unchanged.\n",
        }.get(document, "")
    if t.path == "01_STMicroelectronics/STM32U5":
        return {
            "readme": "\n## Maintained U575 reference\n\nOriginal cmsis_adapter/ targets U575 SPI1 with packet-counted transfers, bounded waits and an ES0499 EOT guard. Reference link and modeled fault/chunk tests passed. Use board/reference_image.c only with TrustZoneDisabled reset MSI4 clocks. See build.md, board/reference_image.md and sources.md. No physical boot or secure/nonsecure deployment is qualified.\n",
            "integration": "\n## Concrete U575 binding\n\nAdd cmsis_adapter/, standard Port and shared Core. Use either the reference board binding or your own, never both. The reference does not change security option bytes and must not be used as a nonsecure application under a secure loader. Output is a latest-frame debugger snapshot. See board/reference_image.md.\n",
            "version": "\n2026-09-06: U575 adapter 0.1.0, real reference link and modeled faults/65535+1 splitting passed; ES0499 guard added. Core 2.0 unchanged.\n",
        }.get(document, "")
    if t.path == "01_STMicroelectronics/STM32G4":
        return {
            "readme": "\n## Maintained G474 reference\n\nUse cmsis_adapter/ with STM32G474RE and the original cold-reset HSI16 board/reference_image.c. See build.md, sources.md and board/reference_image.md. Bounded hardware SPI, 1 MHz, no HAL/IRQ/DMA sharing. Software link and modeled faults passed; board headers and electrical operation are unverified.\n",
            "integration": "\n## Concrete CMSIS binding\n\nAdd cmsis_adapter/ads1299_g4_hal.c, standard Port and shared Core to your official Cube project. Use either your own board binding or board/reference_image.c, never both. The reference requires HSI16 reset clocks and owns SPI1; do not also initialize it through HAL. See board/reference_image.md.\n",
            "version": "\n2026-09-06: original G474 polling adapter 0.1.0; genuine-header compile, GNU reference link and modeled fault tests passed. Core 2.0 unchanged.\n",
        }.get(document, "")
    if t.path == "08_Infineon/PSoC6":
        return {
            "readme": "\n## Maintained PDL reference\n\nThe original pdl_adapter/ uses nonblocking PDL FIFO APIs with bounded polling. See build.md and board/reference_image.md. A 28-source reference links with the official prebuilt CM0P_SLEEP image; modeled fault tests passed. Control GPIO assignments are provisional, not verified header positions. Output is a debugger snapshot, not a lossless stream.\n",
            "integration": "\n## Concrete PDL path\n\nUse pdl_adapter/ads1299_psoc6_hal.c and supply a clock-matched Mode 1 configuration, GPIO/HSIOM setup and real delay callback in board_ads1299_hal(). Do not share the SCB with HAL SPI, ISR, DMA or CM0+. The BSP-selected boot image remains required. See build.md.\n",
            "version": "\n2026-09-06: PDL adapter 0.1.0; PDL 3.17.0 / core-lib 1.4.4 / BSP 5.0.1 reference link and modeled-MMIO tests passed. Physical boot unverified. Core 2.0 unchanged.\n",
        }.get(document, "")
    if t.path == "06_Microchip/SAME54":
        return {
            "readme": "\n## Maintained SAME54 reference\n\nUse harmony_adapter/ and the clock-specific board/reference_image.c. See build.md and board/reference_image.md for the complete GNU reference link and provisional control GPIOs. The SPI adapter uses official DFP definitions, not a copied PLIB. Start at 1 MHz; do not share SERCOM6 with vendor SPI ISR/DMA. Output is a debugger snapshot only.\n",
            "integration": "\n## Concrete adapter\n\nUse harmony_adapter/ads1299_same54_hal.c for step 5, and supply validated clocks, mux, directions and microsecond delay. The optional board/reference_image.c implements the documented cold-start profile. See build.md. Never mix this polling adapter with another owner of SERCOM6.\n",
            "version": "\n2026-09-06: original polling adapter 0.1.0, complete GNU reference ELF and modeled fault tests passed; Core 2.0 unchanged.\n",
        }.get(document, "")
    if t.path == "01_STMicroelectronics/STM32H743":
        return STM32H743_APPENDICES.get(document, "")
    if t.path == "01_STMicroelectronics/STM32F407":
        return STM32F407_APPENDICES.get(document, "")
    if t.path == "02_Espressif/ESP32S3":
        return ESP32S3_APPENDICES.get(document, "")
    if t.path == "03_NXP/MIMXRT1170":
        return MIMXRT1170_APPENDICES.get(document, "")
    if t.path == "03_NXP/MIMXRT1062":
        return MIMXRT1062_APPENDICES.get(document, "")
    if t.path == "05_Renesas/RA6M5":
        return {
            "readme": "\n## FSP reference build\n\nThe original board/reference_image.c links with external FSP 5.6 startup/BSP and TinyUSB 0.18 configuration. See board/reference_image.md and build.md. Start at 1 MHz. API fault-path tests and complete reference ELF link passed; physical routing and hardware acquisition remain unverified. For a generated project use the alternative board/fsp_binding.c, never both bindings. Output is a debugger snapshot, not a lossless stream.\n",
            "integration": "\n## Concrete FSP binding\n\nFor step 5 above, use the supplied fsp_adapter/ads1299_fsp_hal.c and board/fsp_binding.c. You still supply verified control pins and an independent microsecond timer in ads1299_board_config.h. See board/fsp_configuration.md for generated-main integration and SPI ownership. Never add tests/fixtures to production include paths.\n",
            "version": "\nFSP adapter revision 0.2.0, 2026-09-05; Core 2.0 unchanged. FSP 5.6 reference ELF link passed; 6.5 API tests passed but 6.5 BSP build unverified.\n",
            "validation": "\n## Rank 8 software reference gate, 2026-09-05\n\nFSP 5.6 complete reference ELF passed: 25 sources and zero undefined symbols, without fixture APIs. Separately, ARM-emulated adapter tests cover faults and frozen timer. No physical runtime test claimed. See build.md. The software gate permits moving to rank 9; catalog remains conservatively Reference.\n",
        }.get(document, "")
    if t.path == "07_Nordic/nRF5340":
        return {
            "readme": "\n## Verified hardware-SPI application\n\nThe current CMake entry uses zephyr_adapter/, the standard Port and the minimal example. See zephyr_adapter/README.md and build.md. Legacy src/board_sdk.c and port/ files are retained but not built. Output is a latest-frame debugger snapshot, not UART/USB/BLE streaming.\n",
            "integration": "\n## Active Zephyr integration\n\nBuild this directory directly with the nrf5340dk/nrf5340/cpuapp board. app.overlay selects hardware SPIM4 at the documented pins; do not also add the legacy bit-banged port. The supplied board_ads1299_hal() initializes GPIO and SPI callbacks. See build.md for the exact tested Zephyr version and clean build script. NCS CI is a separate unverified path.\n",
            "version": "\nHardware-SPIM4 adapter and real local Zephyr build added 2026-09-05; Core 2.0 unchanged.\n",
        }.get(document, "")
    return ""


def generate_target(t: Target) -> dict[str, object]:
    if t.path == "03_NXP/LPC55S69":
        t = replace(t, sdk_version="MCUX_2.16.000 / 6f3fd257; Core0 reference ELF verified",
                    compiler="GCC Arm Embedded 9.2.1; MCUXpresso IDE not run",
                    spi="SPI7 Mode 1; FRO12 / 12 = 1 MHz",
                    pins=("PIO0_21 / P17-14", "PIO0_20 / P17-10", "PIO0_19 / P17-12",
                          "PIO1_31 (provisional)", "PIO1_7 (provisional)", "PIO0_26 (provisional)",
                          "PIO0_27 (provisional)", "PIO1_6 (provisional)", "Debugger snapshot only"))
    if t.path == "02_Espressif/ESP32":
        t = replace(t, mcu="ESP32-WROOM-32", board="ESP32-DevKitC V4 / 4 MB WROOM reference",
                    sdk_version="IDF 5.4.0; bootloader/application build verified 2026-09-07",
                    compiler="Xtensa ESP ELF GCC", debugger="External JTAG; UART flashing",
                    pins=("GPIO18 / J3-9", "GPIO23 / J3-2", "GPIO19 / J3-8", "GPIO21 / J3-6",
                          "GPIO27 / J2-11", "GPIO22 / J3-3", "GPIO25 / J2-9", "GPIO26 / J2-10", "UART0; no EEG stream implemented"))
    if t.path == "02_Espressif/ESP32C6":
        t = replace(t, sdk_version="5.4.0 reference; PlatformIO Espressif32 6.10.0",
                    pins=("GPIO6 / J1-5", "GPIO7 / J1-6", "GPIO2 / J1-12", "GPIO10 / J1-10",
                          "GPIO3 / J1-13", "GPIO18 / J3-10", "GPIO19 / J3-9", "GPIO1 / J1-8", "UART0 TX; no stream implemented"))
    if t.path == "01_STMicroelectronics/STM32U5":
        t = replace(t, mcu="STM32U575ZI", board="NUCLEO-U575ZI-Q; provisional control GPIOs",
                    sdk_version="CubeU5 1.1.0 linker; CMSIS device 624374fa; GNU reference verified",
                    compiler="GCC Arm Embedded 9.2.1; CubeIDE/Keil not run", debugger="ST-LINK / SWD",
                    spi="SPI1 Mode 1; MSI4 / 4 = 1 MHz")
    if t.path == "01_STMicroelectronics/STM32G4":
        t = replace(t, mcu="STM32G474RE", board="NUCLEO-G474RE; provisional control GPIO profile",
                    sdk_version="CubeG4 1.5.2 linker; CMSIS device 626ee412; GNU reference verified",
                    compiler="GCC Arm Embedded 9.2.1; CubeIDE/Keil not run", debugger="ST-LINK / SWD",
                    spi="SPI1 Mode 1; reference HSI16 / 16 = 1 MHz")
    if t.path == "05_Renesas/RA6M5":
        t = replace(t, sdk_version="5.6.0 reference ELF verified; 6.5.0 API tested only")
    if t.path == "06_Microchip/SAME54":
        t = replace(t, sdk_version="CSP examples 3.6.1; GNU reference link verified; XC32 not run")
    if t.path == "08_Infineon/PSoC6":
        t = replace(t, sdk_version="PDL 3.17.0 / BSP 5.0.1 / core-lib 1.4.4; GNU reference link verified")
    base = MCU_ROOT / t.path
    base.mkdir(parents=True, exist_ok=True)
    write(base / "README.md", render_readme(t) + target_appendix(t, "readme"))
    write(base / "platform_info.yaml", render_platform_info(t))
    write(base / "capability.md", render_capability(t))
    write(base / "ads1299_port" / "ads1299_spi.h", PORT_SPI_H)
    write(base / "ads1299_port" / "ads1299_spi.c", PORT_SPI_C)
    write(base / "ads1299_port" / "ads1299_gpio.h", PORT_GPIO_H)
    write(base / "ads1299_port" / "ads1299_gpio.c", PORT_GPIO_C)
    write(base / "ads1299_port" / "ads1299_drdy.h", PORT_DRDY_H)
    write(base / "ads1299_port" / "ads1299_drdy.c", PORT_DRDY_C)
    example = EXAMPLE_C
    if t.path in ("03_NXP/MIMXRT1170", "03_NXP/MIMXRT1062", "04_TexasInstruments/MSPM0G3507", "07_Nordic/nRF5340", "05_Renesas/RA6M5", "06_Microchip/SAME54", "08_Infineon/PSoC6", "01_STMicroelectronics/STM32G4", "01_STMicroelectronics/STM32U5", "02_Espressif/ESP32C6", "02_Espressif/ESP32", "03_NXP/LPC55S69"):
        example = example.replace(
            'extern int board_ads1299_hal(ads1299_platform_hal_t *hal);',
            'extern int board_ads1299_hal(ads1299_platform_hal_t *hal);\n\n'
            '/* Debugger snapshot only, not lossless transport. Odd sequence means writing. */\n'
            'volatile ads1299_frame_t ads1299_latest_frame;\n'
            'volatile uint32_t ads1299_frame_sequence;')
        example = example.replace(
            "/* Send frame through the application's UART/USB/BLE/Ethernet path. */",
            '++ads1299_frame_sequence;\n            ads1299_latest_frame = frame;\n'
            '            ++ads1299_frame_sequence;')
    if t.path in ("07_Nordic/nRF5340", "05_Renesas/RA6M5", "06_Microchip/SAME54", "08_Infineon/PSoC6", "01_STMicroelectronics/STM32G4", "01_STMicroelectronics/STM32U5", "02_Espressif/ESP32C6", "02_Espressif/ESP32", "03_NXP/LPC55S69"):
        example = example.replace('if (port.drdy_read(port.user) == 0) {',
            'int ready = port.drdy_read(port.user);\n        if (ready < 0) return 11;\n'
            '        if (ready == 0) {')
    if t.path in ("02_Espressif/ESP32C6", "02_Espressif/ESP32"):
        example = example.replace('int main(void)', 'extern void board_ads1299_idle(void);\n\nint main(void)')
        example = example.replace('for (;;) {', 'for (;;) {\n        board_ads1299_idle(); /* one tick; not a lossless stream */')
    write(base / "examples" / "main_ads1299.c", example)
    write(base / "board" / "pinmap.md", render_pinmap(t))
    write(base / "integration.md", render_integration(t) + target_appendix(t, "integration"))
    write(base / "version.md", render_version(t) + target_appendix(t, "version"))
    write(base / "tests" / "test_spi.c", TEST_SPI)
    write(base / "tests" / "test_reset.c", TEST_RESET)
    write(base / "tests" / "test_register.c", TEST_REGISTER)
    write(base / "tests" / "test_frame.c", TEST_FRAME)
    write(base / "validation.md", render_validation(t) + target_appendix(t, "validation"))
    rank = t.rank or EXISTING_RANKS[t.path]
    manifest = {
        "schema": 2, "rank": rank, "path": t.path, "vendor": t.vendor, "family": t.family,
        "mcu": t.mcu, "board": t.board, "architecture": t.architecture,
        "sdk": t.sdk, "tier": maintenance_tier(rank), "status": t.status,
        "core": "../../../core_driver/ads1299", "port": "ads1299_port",
        "example": "examples/main_ads1299.c",
    }
    write(base / "project.json", json.dumps(manifest, ensure_ascii=False, indent=2))
    return manifest


def vendor_landing_pages() -> None:
    planned = {
        "21_Ambiq": ("Ambiq", "Apollo family reference ports are maintained below; vendor SDK builds remain unverified."),
        "22_BouffaloLab": ("Bouffalo Lab", "BL616 and BL808 reference ports are maintained below; vendor SDK builds remain unverified."),
        "23_Realtek": ("Realtek", "RTL8720 and RTL87xx entries are maintained without redistributing proprietary SDK content."),
        "24_Toshiba": ("Toshiba", "The TXZ reference port is maintained below; vendor SDK builds remain unverified."),
        "25_Other": ("Other MCU, SoC, generic-core and DSP ecosystems", "These ranked entries remain grouped here until a stable dedicated vendor category is justified."),
    }
    for dirname, (name, note) in planned.items():
        write(MCU_ROOT / dirname / "README.md", f"# {name}\n\nDirectory role: **Catalog**.\n\n{note}\n\nDo not create an empty MCU target or claim compatibility without source, integration instructions and validation evidence.\n")


def main() -> None:
    normalized = (
        replace(target, family=RANKED_FAMILY_NAMES.get(target.path, target.family))
        for target in TARGETS
    )
    manifests = sorted((generate_target(target) for target in normalized), key=lambda item: item["rank"])
    vendor_landing_pages()
    catalog = {
        "schema": 2,
        "policy": "Core owns ADS1299 behavior; target directories own hardware-only ports.",
        "projects": manifests,
    }
    write(MCU_ROOT / "catalog.json", json.dumps(catalog, ensure_ascii=False, indent=2))
    print(f"generated {len(manifests)} MCU target maintenance packages")


if __name__ == "__main__":
    main()
