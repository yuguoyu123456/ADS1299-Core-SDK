# Common ADS1299 driver

This directory is intentionally platform-independent. MCU/FPGA-specific GPIO, SPI, interrupts and delays belong in `ports/` or complete board projects under `projects/`.

## Driver contract

The port provides:

- full-duplex SPI transfer
- chip-select control
- optional RESET control
- optional START control
- optional DRDY read
- microsecond delay

The ADS1299 data frame is 216 bits = 24-bit status + 8 x 24-bit channel samples.

## Multi-device project policy

The primary multi-ADS1299 architecture in this repository is **standard/cascaded SPI with one independent CS per ADS1299**.

For multiple converters, projects should normally:

- share `SCLK`;
- share `DIN/MOSI`;
- share `DOUT/MISO` where the board follows the TI cascaded topology;
- allocate one `CS` per ADS1299;
- use a common clock and synchronized START strategy for simultaneous sampling;
- keep per-device `DRDY` available during bring-up when GPIO permits;
- instantiate one `ads1299_t` handle per physical converter.

This gives independent register access and easier fault isolation while preserving a common SPI peripheral. TI describes this cascaded configuration as suitable for the majority of applications.

Daisy-chain mode remains a supported ADS1299 chip feature, but it is **optional** and is not the default architecture for this project's 16/32/64-channel reference systems.

See [`../../docs/MULTI_ADS1299_SPI_ARCHITECTURE.md`](../../docs/MULTI_ADS1299_SPI_ARCHITECTURE.md).

## Important

This code is currently **code-reviewed / pre-hardware**. It must not be labeled `Bench-tested` until it passes the repository acceptance procedure on real hardware. SPI mode, timing, synchronization, bus bandwidth and power-up sequencing must be checked against the TI ADS1299 datasheet for the target board.
