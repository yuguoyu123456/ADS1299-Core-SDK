# Common ADS1299 driver

This directory is intentionally platform-independent. MCU/FPGA-specific GPIO, SPI and delays belong in `ports/` or `examples/`.

## Driver contract

The port provides:

- full-duplex SPI transfer
- chip-select control
- optional RESET control
- optional START control
- optional DRDY read
- microsecond delay

The ADS1299 data frame is 216 bits = 24-bit status + 8 x 24-bit channel samples.

## Important

This code is currently **code-reviewed / pre-hardware**. It must not be labeled `Bench-tested` until it passes the repository acceptance procedure on real hardware. SPI mode, timing and power-up sequencing must be checked against the TI ADS1299 datasheet for the target board.
