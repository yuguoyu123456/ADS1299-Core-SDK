# MSPM0 DriverLib adapter

This bridge maps the generic hardware HAL to TI DriverLib. SysConfig remains
responsible for clocks, SPI1 Mode 1, and GPIO initialization. PB6 is configured
as ordinary GPIO so Core controls physical CS across complete ADS1299 commands.

Initialize SPI1 for 8-bit controller operation at 4 MHz, populate
`ads1299_mspm0_ctx_t` with SysConfig symbols, and call
`ads1299_mspm0_make_hal()`. DMA is deliberately outside this polling reference.

The optional `board/launchpad_reference.c` supplies cold-reset initialization
without SysConfig-generated files in this repository. Its conservative SPI
rate is 1 MHz. Use the same adapter with your own SysConfig initialization
instead when other peripherals are present; never run both initializers.

TX-full/RX-empty/SPI-busy loops are limited by ADS1299_MSPM0_POLL_LIMIT
(default 100000 iterations per wait, not microseconds). Stale RX data and
timeouts return -1. Stop acquisition and reset/reinitialize before retrying;
do not publish partial frames. Context and peripheral ownership must persist.
GPIO masks must be single-bit and distinct on each port; DRDY is read-only.
The delay callback splits large requests and never passes zero or one cycles
to TI's delay routine. Delay accuracy and polling budget require board tests.
