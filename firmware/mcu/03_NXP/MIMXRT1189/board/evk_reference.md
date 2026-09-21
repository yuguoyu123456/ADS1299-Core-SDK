# MIMXRT1180-EVK reference wiring contract

Validation status: **TEMPLATE / NOT BUILD-VERIFIED / NOT BOARD-VERIFIED**.

The preferred reference board for this model leaf is NXP **MIMXRT1180-EVK**, whose fitted processor is MIMXRT1189CVM8C. This file deliberately records only board facts that can be traced to current NXP documentation; it does not invent a complete ADS1299 pin assignment.

## Verified SPI breakout

NXP application note AN14393 (Rev. 1.0, 4 September 2024), Table 5, identifies these MIMXRT1180-EVK U13 connections:

| ADS1299 role | EVK signal | Connector |
|---|---|---|
| CS (candidate) | LPSPI1_PCS0 | U13-1 |
| DOUT -> MCU | LPSPI1_MISO | U13-2 |
| DIN <- MCU | LPSPI1_MOSI | U13-5 |
| SCLK | LPSPI1_SCK | U13-6 |
| Ground | GND | J37-20 (AN14393 Table 6) |

For this SDK, configure the LPSPI controller as **SPI Mode 1 (CPOL=0, CPHA=1), MSB-first**. Prefer software-controlled GPIO CS in the board binding unless the selected MCUXpresso implementation has been explicitly validated with hardware PCS timing.

## Control signals still require a board binding

ADS1299 `DRDY`, `START`, `RESET` and `PWDN` are not assigned here. Select free 3.3-V-capable GPIOs in the actual MCUXpresso project after checking the current EVK schematic/pin tool for conflicts. Put those choices only in the board/config layer that implements `board_ads1299_hal()`; do not edit the shared ADS1299 core.

This is intentional: the RT1180 EVK exposes many functions and NXP documentation has changed during the board's lifetime. A made-up Arduino/GPIO mapping would be less useful than an explicit incomplete contract. Record the chosen pad, GPIO instance/index, connector pin and conflict check before treating a local binding as reproducible.

## Bring-up checklist

1. Start from an official MIMXRT1180-EVK MCUXpresso SDK project for the core you intend to run.
2. Verify U13 LPSPI1 routing against the SDK pin tool / generated pin mux used by that project.
3. Configure LPSPI for Mode 1, MSB-first, conservative clocking for initial ADS1299 bring-up.
4. Assign and configure DRDY/START/RESET/PWDN in the board layer; DRDY must be readable without blocking transport work.
5. Share digital ground and verify logic-voltage compatibility before connection. Do not infer ADS1299 analog-supply wiring from this digital-interface note.
6. Run the SDK sequence: probe ID -> internal test -> input short -> 250-SPS EEG -> bounded stream.
7. Promote status to BUILD-VERIFIED only after the documented software configuration actually compiles; promote to BOARD-VERIFIED only after physical-board evidence exists.

## Sources

- NXP, *i.MX RT1180 Evaluation Kit* product page: MIMXRT1180-EVK uses MIMXRT1189CVM8C and provides an Arduino expansion interface.
- NXP AN14393, *Using Serial Downloader Option in i.MX RT1180 via LPSPI*, Rev. 1.0, 4 September 2024, Tables 5-6: U13 LPSPI1 and ground connector facts above.
- NXP IMXRT1180QSG, *Quick Start Guide i.MX RT1180 Evaluation Kit*, Rev. 2, 17 June 2025.
- NXP UM12021, *MIMXRT1180-EVK Board User Manual*, Rev. 3.0, 12 August 2025; use the current manual/schematic and MCUXpresso pin tool when selecting the remaining GPIOs.
