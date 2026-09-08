# Generic RISC-V RV32 ADS1299 Ready-to-Run Guide

This file is additive; existing target files remain unchanged.

Configure the concrete RV32 platform SPI peripheral as ADS1299 mode 1 (CPOL=0, CPHA=1), start around 2-4 MHz, and map CS/RESET/PWDN/START plus falling-edge DRDY through the existing callbacks. Validate `ID -> internal test -> input short -> 250 SPS EEG` first.

For ADS1299-8, service one complete 27-byte RDATAC frame per DRDY and place it in a bounded ring buffer before transport/application work.

Generic RV32 targets vary in interrupt controller, DMA, cache and memory architecture. Keep these platform-specific. Prove blocking SPI first, then add DMA/non-blocking transfer where supported. For 8 x ADS1299 / 64 channels, use a capable multi-lane SoC or the repository FPGA capture layer for deterministic alignment.

See `../DIRECT_USE.md` and `../_shared/other_ads1299_ready_app.[ch]`.
