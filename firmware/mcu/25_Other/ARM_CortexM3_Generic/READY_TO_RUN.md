# Generic Cortex-M3 ADS1299 Ready-to-Run Guide

Additive guide; existing files remain unchanged.

Configure SPI mode 1 (CPOL=0, CPHA=1), begin around 2-4 MHz, and map CS/RESET/PWDN/START plus falling-edge DRDY through the existing port callbacks. Validate `ID -> internal test -> input short -> 250 SPS EEG` before raising the data rate.

For ADS1299-8, read exactly one 27-byte RDATAC frame per DRDY. Recommended flow: `DRDY -> acquisition event -> SPI read -> frame validation -> ring buffer -> transport`.

Cortex-M3 targets are suitable for a deterministic single-ADS1299 node. Keep memory bounded and first prove blocking SPI. Add DMA where the concrete MCU supports it. For 8 x ADS1299 / 64 channels, use multiple deterministic lanes only on suitably equipped devices; otherwise prefer the repository FPGA capture layer.

See `../DIRECT_USE.md` and `../_shared/other_ads1299_ready_app.[ch]`.
