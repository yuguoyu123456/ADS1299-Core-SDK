# STM32CubeF4 adapter

This hardware-only adapter binds STM32CubeF4 HAL SPI/GPIO/timing calls to the
stable MCU callback contract. It contains no ADS1299 register or configuration
logic. The reference uses polling; DMA requires a separate measured design.

STM32F407G-DISC1 shares SPI1 with the on-board LIS3DSH accelerometer. The
application must configure PE3 as an output and hold it high whenever ADS1299
owns PA5/PA6/PA7.
