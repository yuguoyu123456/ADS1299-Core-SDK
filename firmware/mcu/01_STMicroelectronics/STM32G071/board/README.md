# STM32G071 board layer

Reference board: **NUCLEO-G071RB (MB1360, STM32G071RBT6)**.

Validation status: **TEMPLATE / integration configuration present**. The mapping below is based on ST's current NUCLEO-G071RB documentation, but this repository does not yet record a clean STM32CubeIDE build or physical ADS1299 board run for this exact configuration.

## What a beginner edits

For the reference board, start with `board_config.h`. When using another board with the same STM32G071 MCU, keep the shared ADS1299 core unchanged and normally change only:

1. CubeMX pin/peripheral routing;
2. `board_config.h` pin/peripheral choices;
3. the model-local HAL binding only if your generated handle names differ.

Do **not** edit `firmware/core_driver/ads1299/ads1299.c`, register definitions, model logic, or frame logic just to bring up a board.

## Reference wiring

| ADS1299 signal | NUCLEO-G071RB | Arduino header | Cube function |
|---|---|---|---|
| SCLK | PA5 | D13 | SPI1_SCK |
| DOUT / MISO | PA6 | D12 | SPI1_MISO |
| DIN / MOSI | PA7 | D11 | SPI1_MOSI |
| CS | PB0 | D10 | GPIO output |
| DRDY | PC7 | D9 | GPIO input / EXTI candidate |
| RESET | PA9 | D8 | GPIO output |
| PWDN | PA8 | D7 | GPIO output |
| START | PB14 | D6 | GPIO output |
| host TX | PA2 | ST-LINK VCP | UART2_TX |
| host RX | PA3 | ST-LINK VCP | UART2_RX |
| GND | GND | GND | common digital ground |

PA5 is also connected to the Nucleo USER LED circuitry. The mapping is convenient for first bring-up, but a custom board may choose another verified SPI routing.

## CubeMX / CubeIDE starter configuration

Configure SPI1 as master, full duplex, 8-bit, **CPOL Low / CPHA second edge (SPI Mode 1)**, MSB first, and keep the SPI clock within the ADS1299 datasheet limit for the electrical design. Configure PB0, PA9, PA8 and PB14 as push-pull outputs. Configure PC7 as an input initially; EXTI can be introduced when moving from polling to interrupt-driven acquisition. Configure UART2 PA2/PA3 for the ST-LINK VCP and use **460800 baud** for the canonical 49-byte packet stream at 250 SPS.

The starter stream needs `49 bytes * 250 frames/s = 12,250 bytes/s`. 115200 baud with 8N1 provides only about 11,520 payload bytes/s and therefore is not sufficient for an unthrottled full canonical stream. 460800 baud provides adequate margin for bring-up.

## Small-MCU acquisition rule

STM32G071 integrations should prefer static bounded storage. Do not allocate from the heap or perform floating-point conversion in the DRDY timing path. For sustained capture, keep DRDY/ISR work short, enqueue raw frames into a bounded ring/queue, process or transmit them outside the timing-critical path, and count overflow explicitly.

## Expected progression

The model folder should ultimately expose: probe/ID -> internal test -> input short -> 250-SPS EEG -> canonical stream -> clean stop. Board-specific code owns pins, handles, clocks and transport only; ADS1299 register/profile behavior belongs to the shared core.
