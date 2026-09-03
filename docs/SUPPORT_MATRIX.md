# Platform support matrix

The project uses one common ADS1299 driver plus thin platform ports. A platform is **not** called supported just because a folder exists.

| Platform family | Status | Representative target | Notes |
|---|---|---|---|
| ST STM32 | Planned | F103/F407/G4/H7 | HAL first, LL later |
| GigaDevice GD32 | Planned | GD32F4 / GD32E5 | SPL/HAL-style port |
| WCH CH32 | Planned | CH32V203 / CH32V307 | RISC-V priority |
| Espressif ESP32 | Planned | ESP32-S3 | ESP-IDF |
| Nordic nRF | Planned | nRF52840 / nRF5340 | nrfx / Zephyr path |
| Raspberry Pi MCU | Planned | RP2040 / RP2350 | Pico SDK |
| NXP | Planned | MCX / i.MX RT | MCUXpresso |
| Microchip 32-bit | Planned | SAM / PIC32 | MPLAB ecosystem |
| Renesas | Planned | RA / RX | FSP / e2 studio |
| Texas Instruments MCU | Planned | MSPM0 / TM4C / CC13xx-26xx | representative ports |
| Infineon | Planned | XMC / PSoC | ModusToolbox where appropriate |
| Silicon Labs | Planned | EFM32 / EFR32 | Gecko SDK |
| HDSC | Planned | HC32 | China ecosystem |
| Artery | Planned | AT32 | China ecosystem |
| MindMotion | Planned | MM32 | China ecosystem |
| Nationstech | Planned | N32 | China ecosystem |
| Puya | Planned | PY32 | low-cost ecosystem |
| Geehy | Planned | APM32 | China ecosystem |
| STC | Planned | STC8 / STC32 | low-rate/basic examples |
| Microchip AVR | Planned | ATmega / AVR Dx | educational/basic examples |

## FPGA track

- AMD/Xilinx: Planned
- Intel/Altera: Planned
- Lattice: Planned

## Status vocabulary

- **Planned**: roadmap only
- **Compiles**: toolchain build verified
- **Bench-tested**: real ADS1299 hardware verified
- **24h-tested**: long-run capture verified

## Minimum hardware acceptance

1. Read the device ID.
2. Read/write registers.
3. Acquire the internal test signal on all 8 channels.
4. Decode the 216-bit frame (24-bit status + 8 x 24-bit channels).
5. Verify 24-bit two's-complement sign extension.
6. Verify 250 SPS; document any higher tested rates.
7. Export the common host packet format.
8. Run the Python viewer/recorder.
