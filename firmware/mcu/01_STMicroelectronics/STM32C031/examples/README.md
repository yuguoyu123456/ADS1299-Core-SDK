# STM32C031 ADS1299 examples

This directory contains the repository-owned runnable ADS1299 bring-up layer for an STM32Cube-generated STM32C031 project. It intentionally does **not** replace CubeMX/CubeIDE startup, clock, linker, or peripheral-generation files.

## Files

- `stm32c031_example_platform.c/.h` — concrete STM32 HAL glue for the repository `ads1299_port_t` adapter and UART output.
- `stm32c031_beginner_demo.c/.h` — complete beginner sequence: probe/ID -> internal test -> input short -> 250-SPS EEG configuration -> canonical packet streaming.
- `main_ads1299.c` — older minimal example kept for compatibility; its historical `board_ads1299_hal()` entry is now supplied by `stm32c031_example_platform.c`.

## Required repository sources

Add these repository-owned sources to the CubeIDE project:

1. `firmware/core_driver/ads1299/` core sources required by `ads1299.h`.
2. `firmware/common/data_packet/ads1299_packet.c`.
3. `firmware/mcu/01_STMicroelectronics/STM32C031/ads1299_port/ads1299_spi.c` and its companion GPIO/DRDY files used by the port.
4. `stm32c031_example_platform.c`.
5. `stm32c031_beginner_demo.c`.

Add include paths for those folders plus `../board`.

## CubeMX / CubeIDE setup

Use `../board/board_config.h` as the only beginner board-configuration file. Generate GPIO labels exactly as documented there:

- `ADS1299_CS`
- `ADS1299_DRDY`
- `ADS1299_RESET`
- `ADS1299_PWDN`
- `ADS1299_START`

Configure the selected SPI peripheral as master, 8-bit, **SPI Mode 1** (`CPOL=Low`, `CPHA=2Edge`) and MSB first. The repository core uses ADS1299 command/register semantics; do not edit core register files for board bring-up.

For the default UART stream path, configure the UART handle named by `ADS1299_BOARD_UART_HANDLE`. For a 49-byte packet at 250 packets/s, use at least 230400 baud for the simple blocking beginner transport; a production design should use interrupt/DMA plus bounded buffering.

## Call from generated `main.c`

Keep Cube-generated initialization intact. After GPIO, SPI and UART initialization, add:

```c
#include "stm32c031_beginner_demo.h"

/* after MX_GPIO_Init(), MX_SPI1_Init(), MX_USART1_UART_Init(), etc. */
(void)stm32c031_ads1299_beginner_demo(1000u);
```

`1000u` streams 1000 canonical packets and then stops cleanly. Pass `0u` for continuous streaming.

## Expected text sequence before binary streaming

A correctly communicating ADS1299 should produce a sequence similar to:

```text
ADS1299 STM32C031 beginner demo
OK ID ADS1299-8
RUN internal-test
OK internal-test frames
RUN input-short
OK input-short frames
RUN EEG 250 SPS config
OK EEG 250 SPS configured
RUN stream canonical 49-byte packets
```

After the final line, the same UART becomes a binary packet stream, so a text terminal will show non-printable characters. Each packet is the repository canonical 49-byte packet produced by `ads1299_packet_encode()`.

The internal-test and input-short stages currently verify successful configuration plus acquisition of consecutive valid frames; they do not claim analog amplitude/noise limits without physical-board measurement.

## Main failure messages

- `ERR RESET/check RESET PWDN power` — inspect power rails, PWDN and RESET wiring/configuration.
- `ERR SDATAC/check SPI Mode 1 CS` — check CPOL/CPHA, CS polarity and SPI peripheral selection.
- `ERR ID/not ADS1299 family; check SPI Mode 1 power clock` — SPI transaction completed but the returned ID was not a recognized ADS1299-family ID.
- `ERR DRDY timeout/check DRDY wiring` — conversion data-ready was not observed active-low within the timeout.
- `ERR frame read/check SPI and CS` — DRDY occurred but the continuous frame transaction failed.
- `ERR stream transport/check UART config` — packet encoding succeeded but UART output failed or is disabled.

## ADS1299 behavior used here

The sequence uses existing typed shared-core APIs rather than raw register literals:

- `ads1299_read_device_id()`
- `ads1299_configure_internal_test()`
- `ads1299_configure_input_short_test()`
- `ads1299_set_data_rate(..., ADS1299_DR_250SPS)`
- `ads1299_set_channel(..., ADS1299_GAIN_24, ADS1299_MUX_NORMAL, ...)`
- `ads1299_wait_drdy()` / `ads1299_read_frame_continuous()`
- `ads1299_packet_encode()`

The 250-SPS setting, selectable gains, internal test source and ADS1299-family capabilities are consistent with the TI ADS1299-x Rev. C datasheet. Hardware-specific electrical behavior still depends on the actual board and analog front-end.

## Validation status

**TEMPLATE / INTEGRATION-COMPLETE FOR THIS LEAF, NOT BOARD-VERIFIED.** The repository-owned HAL glue and complete beginner call path are present, but this change has not been compiled with a concrete generated STM32C031 Cube project in this run and has not been exercised on physical ADS1299 hardware. Do not interpret this as bench, EMC, medical, or production validation.
