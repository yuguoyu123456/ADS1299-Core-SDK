# OpenHW CV32 ADS1299 Ready-to-Run Guide

This file is additive; existing target files remain unchanged.

Configure the concrete CV32-based SoC SPI peripheral as ADS1299 mode 1 (CPOL=0, CPHA=1), start around 2-4 MHz, and map CS/RESET/PWDN/START plus falling-edge DRDY through the existing callback port. Validate `ID -> internal test -> input short -> 250 SPS EEG` before higher-rate operation.

For ADS1299-8, service exactly one 27-byte RDATAC frame per DRDY and enqueue it into a bounded buffer before application processing.

OpenHW CV32 cores are integrated into different SoCs, so interrupt, DMA, cache and memory behavior must remain platform-specific. First prove blocking SPI, then add DMA/non-blocking transfers where supported. For 8 x ADS1299 / 64 channels, use deterministic multi-lane capture only when the concrete SoC supports it; otherwise prefer the repository FPGA capture layer.

See `../DIRECT_USE.md` and `../_shared/other_ads1299_ready_app.[ch]`.
