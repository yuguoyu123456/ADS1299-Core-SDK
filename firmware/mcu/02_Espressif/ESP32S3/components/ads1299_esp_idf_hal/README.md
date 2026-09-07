# ESP-IDF HAL binding

This component maps ESP-IDF SPI/GPIO/delay primitives into the hardware-only
`ads1299_platform_hal_t` contract. It does not construct the Core port and does
not define ADS1299 commands, registers, configuration or transport packets.

The application owns SPI bus/device and GPIO initialization. CS remains a GPIO
because the shared Core controls command and frame transaction boundaries.
