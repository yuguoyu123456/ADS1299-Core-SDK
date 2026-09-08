# T-Head XuanTie ADS1299 Ready-to-Run Guide

This file is additive; existing target files remain unchanged.

Configure the concrete XuanTie-based platform SPI peripheral for ADS1299 mode 1 (CPOL=0, CPHA=1), initially around 2-4 MHz. Route CS, RESET, PWDN, START and falling-edge DRDY through the existing callbacks. Validate `ID -> internal test -> input short -> 250 SPS EEG` first.

For ADS1299-8, read one complete 27-byte RDATAC frame per DRDY and enqueue it into a bounded acquisition buffer.

XuanTie cores span MCU and SoC designs, so interrupt controller, DMA, cache and memory placement remain platform-specific. First prove blocking SPI, then enable DMA/non-blocking transfers. On application-class or multicore systems, isolate acquisition from Linux/AI/network workloads. For 64-channel low-skew capture, prefer the repository FPGA front end.

See `../DIRECT_USE.md` and `../_shared/other_ads1299_ready_app.[ch]`.
