# STM32F407VET6 / Black F407VE — complete reference project

Status: **Source-complete; CI build pending on this branch. Hardware validation pending.**

This is the first gold-reference MCU project. It uses PlatformIO's STM32Cube framework so a clean clone can resolve the ST HAL automatically instead of requiring a manually-generated CubeMX directory.

## Wiring

| ADS1299-Core | STM32F407 | Function |
|---|---|---|
| SCLK | PA5 | SPI1 SCK |
| DOUT/MISO | PA6 | SPI1 MISO |
| DIN/MOSI | PA7 | SPI1 MOSI |
| CS | PB6 | software chip select |
| DRDY | PB7 | falling-edge EXTI |
| RESET | PB8 | active-low reset |
| START | PB9 | start/sync control |
| DGND | GND | digital ground |

UART2 PA2/PA3 is configured at 921600 baud for host output.

## Build

```bash
cd projects/stm32f407_black
pio run
```

Flash with your configured PlatformIO upload method, or copy the generated firmware using ST-LINK/OpenOCD as appropriate for the board.

## Boot behavior

1. initialize clocks/GPIO/SPI/UART;
2. hardware-reset ADS1299;
3. send SDATAC;
4. read and print the ID register;
5. set 250 SPS;
6. route all 8 channels to the ADS1299 internal test signal at gain 24;
7. enter RDATAC and START;
8. each DRDY edge reads one 27-byte ADS1299 frame;
9. emit the repository's 49-byte host packet with sequence, timestamp, raw status, 8 signed int32 samples and CRC16.

The project intentionally boots into **internal test signal**, not human EEG. This is the safer and more reproducible first validation state.

## Expected validation order

`CI-build` only proves compilation. Before `Bench-tested`, capture the ID read and test waveform from a real ADS1299-Core board and attach the evidence to the hardware validation issue.
