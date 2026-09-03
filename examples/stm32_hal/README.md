# STM32 HAL reference example — pre-hardware validation

Recommended first targets: STM32F103 and STM32F407, then G4/H7.

## CubeMX/CubeIDE settings

- SPI master, 8-bit, MSB first
- **CPOL = 0, CPHA = 1** (ADS1299 serial timing)
- start with a conservative SPI clock, then increase only after timing verification
- CS as ordinary GPIO output
- DRDY as GPIO EXTI falling-edge input
- RESET and START as GPIO outputs if routed by the board

## Bring-up order

1. Power rails and reference checked with a DMM/scope.
2. Initialize MCU clocks/GPIO/SPI/UART.
3. `ads1299_init()`.
4. Hardware RESET or RESET command.
5. `SDATAC` before register reads/writes.
6. Read the ID register and print it over UART.
7. Configure internal test signal and verify all eight channels.
8. Enable `RDATAC`, then read one 27-byte frame on each DRDY falling edge.

## Important

The STM32 adapter in `ports/stm32` is generic glue, not a complete Cube project. Pin names and `SPI_HandleTypeDef` come from the user's generated project. This port is not `Bench-tested` until real ADS1299-Core hardware is available.
