# STM32CubeH7 adapter

This adapter binds the repository's hardware-only MCU callbacks to STM32CubeH7
HAL. It contains no ADS1299 register, command or configuration logic.

Supported callback behaviour:

- blocking full-duplex, transmit-only and receive-only SPI transfers;
- explicit software GPIO for CS, RESET, PWDN, START and DRDY;
- microsecond delay from the Cortex-M7 DWT cycle counter;
- bounded splitting before STM32 HAL's 16-bit transfer-length boundary.

The reference build uses polling. A future DMA adapter must use DMA-visible
memory and explicit Cortex-M7 D-cache clean/invalidate operations. Do not label
DMA as tested until a real ADS1299 stream has been measured.

The application owns GPIO/SPI clock and pin initialization. See
`../examples/stm32cube_reference/main.c` for the minimum NUCLEO-H743ZI2 setup.
