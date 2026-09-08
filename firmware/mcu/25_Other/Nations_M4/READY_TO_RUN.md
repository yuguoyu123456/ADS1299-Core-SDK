# Nations M4 ADS1299 Ready-to-Run Guide

This file is additive; existing target files remain unchanged.

Configure SPI for ADS1299 mode 1 (CPOL=0, CPHA=1), start around 2-4 MHz, and route CS/RESET/PWDN/START plus falling-edge DRDY through the existing callbacks. Validate `ID -> internal test -> input short -> 250 SPS EEG` first.

For ADS1299-8, service one complete 27-byte frame per DRDY. Use a high-priority acquisition context, validate the status/header and enqueue into a bounded ring buffer before transport or DSP work.

Generic Cortex-M4 guidance applies: prove blocking SPI first, then add DMA where the concrete device supports it. Keep FPU/DSP processing outside the ISR. For 8 x ADS1299 / 64 channels, use multi-SPI/DMA resources only when verified on the concrete device; otherwise use the repository FPGA capture layer.

See `../DIRECT_USE.md` and `../_shared/other_ads1299_ready_app.[ch]`.
