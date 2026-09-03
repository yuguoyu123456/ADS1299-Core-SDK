# Platform support matrix

The project uses one common ADS1299 driver plus thin platform ports. A platform is **not** called supported just because a folder exists.

| Platform family | Status | Representative target | Notes |
|---|---|---|---|
| ST STM32 | Source-added | F103/F407/G4/H7 | generic Cube HAL adapter; complete board project pending |
| GigaDevice GD32 | Source-added | GD32F4 / GD32E5 | GD32F4xx SPL-style adapter; vendor build pending |
| WCH CH32 | Source-added | CH32V203 / CH32V307 | CH32V30x adapter; vendor build pending |
| Espressif ESP32 | Source-added | ESP32-S3 | ESP-IDF adapter; vendor build pending |
| Nordic nRF | Planned | nRF52840 / nRF5340 | nrfx / Zephyr path |
| Raspberry Pi MCU | Source-added | RP2040 / RP2350 | Pico SDK adapter; vendor build pending |
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

Additional framework adapter: **Arduino — Source-added**, pending representative-board build verification.

## FPGA track

- AMD/Xilinx: common RTL frame-unpack source + simulation smoke test; vendor synthesis project Planned
- Intel/Altera: common RTL reusable; vendor synthesis project Planned
- Lattice: common RTL reusable; vendor synthesis project Planned

## Status vocabulary

- **Planned**: roadmap/folder only
- **Source-added**: concrete adapter source exists but the official vendor toolchain build has not yet been recorded
- **Compiles**: exact target/toolchain build verified
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
