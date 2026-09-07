# Hardware SPIM4 adapter

The current build uses Zephyr SPIM4/EasyDMA, the standard ads1299_port files,
the one shared Core and examples/main_ads1299.c. The older src/board_sdk.c and
port/ files remain for migration reference but are not part of CMake's build.
Do not add them alongside the new adapter.

app.overlay maps SCK/MOSI/MISO to P1.04/P1.06/P1.05 and software CS to P1.07.
DRDY/RESET/PWDN/START are P1.08/P1.09/P1.10/P1.11. Physical pin polarity is
passed unchanged to Core. CS/RESET/PWDN initialize high, START low and DRDY
input. Zephyr must not control CS per transaction: the overlay removes the
board's automatic cs-gpios property, and spi_config has no CS control.

SPI uses 1 MHz, Mode 1, 8-bit, MSB-first. Small RAM TX/RX staging buffers
support EasyDMA and null-TX/null-RX contracts without heap allocation. Transfer
chunks never toggle physical CS. The port is single-owner and not ISR-safe;
SPI4 must not be shared while the Core holds CS. Stop and reinitialize after
an SPI or GPIO error rather than publishing a partial frame.

DRDY is polled. SPI completion uses Zephyr's driver timeout (3.7.1 configuration
adds a 200 ms tolerance); it is not a lossless-acquisition timing guarantee.
No Bluetooth/network-core image or USB/UART stream is provided. The example
exposes a latest-frame debugger snapshot with an even/odd update counter.
The original P1.12 bit-banged serial output is no longer used. Hardware and
long-duration tests remain required before any acquisition-performance claim.
