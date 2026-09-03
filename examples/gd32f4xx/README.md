# GD32F4xx standard peripheral library adapter — source added, not yet compiled

The adapter uses GigaDevice SPL-style APIs including `spi_i2s_flag_get`, `spi_i2s_data_transmit`, `spi_i2s_data_receive`, `gpio_bit_write` and `gpio_input_bit_get`.

Configure SPI as master, 8-bit, MSB-first, CPOL=0/CPHA=1. CS is a normal GPIO. Provide a working microsecond delay callback in `ads1299_gd32f4xx_ctx_t`.

First target for toolchain verification: one GD32F4xx development board. After build verification, update the support matrix to `Compiles`; hardware status remains separate.
