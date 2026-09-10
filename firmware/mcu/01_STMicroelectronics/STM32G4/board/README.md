# STM32G4 ADS1299 board configuration

Reference board: **NUCLEO-G474RE (MB1367, STM32G474RET6)**.

Validation status: **TEMPLATE / integration configuration present**. The repository now provides a concrete reference-board configuration, but this does **not** mean the target project has been build-verified or that ADS1299 operation has been board-verified.

## Beginner edit point

For the repository reference path, edit only:

`board/board_config.h`

when adapting to another STM32G4 board. Keep ADS1299 register/profile/frame behavior in `firmware/core_driver/ads1299/`; a new student should not edit shared core files for ordinary bring-up.

CubeMX/CubeIDE still owns generated clock, GPIO, SPI and UART initialization. This repository file records which peripherals/pins the ADS1299 integration expects.

## Reference wiring

| ADS1299 | NUCLEO-G474RE | Arduino label | Purpose |
|---|---|---|---|
| SCLK | PA5 | D13 | SPI1 SCK |
| DOUT | PA6 | D12 | SPI1 MISO |
| DIN | PA7 | D11 | SPI1 MOSI |
| CS | PB6 | D10 | active-low chip select |
| DRDY | PC7 | D9 | active-low data-ready input |
| RESET | PA9 | D8 | active-low reset |
| PWDN | PA8 | D7 | active-low power-down |
| START | PB10 | D6 | conversion start control |
| stream TX | PA2 | VCP path | LPUART1 TX to STLINK-V3E in the documented default bridge configuration |
| stream RX | PA3 | VCP path | LPUART1 RX from STLINK-V3E in the documented default bridge configuration |

The NUCLEO-G474RE Arduino connector mapping is documented by ST in UM2505. The same manual documents the selectable VCP routing and shows LPUART1 PA2/PA3 connected to STLINK-V3E in the default bridge configuration.

## CubeMX settings expected by the starter path

Configure **SPI1** as full-duplex master, 8-bit, MSB-first, with **CPOL=Low and CPHA=2 Edge** (SPI Mode 1). Use software-controlled GPIO CS rather than relying on hardware NSS unless a later integration explicitly documents and validates another arrangement.

Configure PB6, PA9, PA8 and PB10 as push-pull outputs; configure PC7 as an input or falling-edge EXTI source for DRDY. Start with polling/blocking bring-up, then move sustained acquisition to a short DRDY ISR plus a bounded queue/ring buffer.

For reference-board host streaming, configure **LPUART1 PA2/PA3** and start with **460800 baud, 8N1**. A canonical 49-byte packet at 250 SPS carries 12,250 payload bytes/s, so 115200 baud is not sufficient for a lossless full-packet stream.

## Electrical and safety boundary

Confirm ADS1299 DVDD and MCU GPIO logic compatibility, connect grounds correctly, and verify the exact board revision/solder-bridge state before wiring. Do not infer analog-front-end safety, patient isolation, EMC or production readiness from this digital SDK example. Do not attach people-connected electrodes to an unisolated development setup.

## Portability to another STM32G4 board

A different board using the same MCU family should normally require only:

1. update `board_config.h` pin/peripheral values;
2. regenerate or adjust CubeMX board initialization to match those values;
3. keep the shared ADS1299 core unchanged.

If a board change forces edits to ADS1299 register logic or shared driver internals, treat that as an integration defect rather than the intended beginner workflow.
