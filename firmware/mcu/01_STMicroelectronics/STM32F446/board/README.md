# STM32F446 board layer

## Reference board

Use **NUCLEO-F446RE (MB1136, STM32F446RET6)** as the reference path for this model folder.

This board is a good first target because it provides integrated ST-LINK, Arduino Uno V3 headers, ST Morpho headers, and direct STM32Cube ecosystem support.  The repository-owned board edit point is `board_config.h`.

Validation status for this board layer: **TEMPLATE / integration configuration present**.  The repository pin/config mapping is documented, but this starter mapping is not claimed BOARD-VERIFIED with an ADS1299.

## Beginner rule: edit one place

For first bring-up, configure STM32CubeMX to match `board_config.h`.  If you use another STM32F446 board, change only:

1. CubeMX peripheral/pin routing; and
2. `board/board_config.h` (plus the vendor-generated HAL handles consumed by the platform layer).

Do **not** edit `firmware/core_driver/ads1299/` simply because your board uses different pins.

## Reference wiring

The starter path uses SPI1 on the standard Nucleo/Arduino SPI pins and software-controlled ADS1299 control pins:

| ADS1299 signal | NUCLEO-F446RE STM32 pin | Arduino label | CubeMX role |
|---|---|---|---|
| SCLK | PA5 | D13 | SPI1_SCK |
| DOUT / MISO | PA6 | D12 | SPI1_MISO |
| DIN / MOSI | PA7 | D11 | SPI1_MOSI |
| CS | PB6 | D10 | GPIO output, active low |
| DRDY | PC7 | D9 | GPIO input / EXTI candidate, active low |
| RESET | PA9 | D8 | GPIO output, active low |
| PWDN | PA8 | D7 | GPIO output, active low |
| START | PB10 | D6 | GPIO output |
| stream TX | PA2 | ST-LINK VCP | USART2_TX |
| stream RX | PA3 | ST-LINK VCP | USART2_RX |
| digital ground | GND | GND | common digital ground |

Before applying power, verify the exact ADS1299 module digital I/O voltage and the exact MB1136 board revision/solder-bridge state.  This table describes digital control wiring only; it is not an analog-front-end or patient-safety wiring guide.

## STM32CubeMX settings

Configure SPI1 as:

- Master / full duplex
- 8-bit data size
- **CPOL low, CPHA second edge (SPI Mode 1)**
- MSB first
- software NSS / software-controlled CS
- start with a conservative SPI clock for bring-up; increase only after stable ID/readback behavior

Configure GPIO outputs for CS, RESET, PWDN and START.  Configure DRDY as an input; an EXTI/DMA-based acquisition path can be introduced after basic polling bring-up succeeds.

Configure USART2 for the starter host stream.  On the default MB1136 solder-bridge configuration, PA2/PA3 are connected to the on-board ST-LINK VCP.  If you intentionally reroute those pins to headers, follow the MB1136 solder-bridge documentation rather than assuming the VCP remains connected.

## Expected first-success sequence

Once the model-level progressive example is integrated, the intended order is:

1. power/reset initialization;
2. ADS1299 `SDATAC` / ID probe;
3. internal test profile;
4. input-short profile;
5. 250-SPS EEG profile;
6. DRDY-driven frame reads;
7. canonical packet streaming;
8. clean STOP / SDATAC shutdown.

The shared ADS1299 core must own register/profile semantics; this board layer only owns pins, peripherals, clocks and transport choices.

## High-performance acquisition note

STM32F446 is fast enough that a blocking HAL implementation is acceptable for initial low-rate bring-up, but it is not the final architecture for sustained high-rate or multi-ADS1299 acquisition.  Production-quality acquisition should use bounded buffering with IRQ/DMA as appropriate, keep transport/printf work out of the DRDY critical path, and expose overflow counters instead of silently losing frames.

## Status

- Reference board selected: **yes — NUCLEO-F446RE**
- Single board/config edit point: **yes — `board_config.h`**
- Shared-core modification required for a board pin change: **no**
- Starter mapping BUILD-VERIFIED in STM32CubeIDE: **not yet recorded**
- ADS1299 BOARD-VERIFIED on NUCLEO-F446RE: **not yet recorded**
