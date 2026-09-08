# Nuclei RISC-V ADS1299 Ready-to-Run Guide

This file is additive; existing target files remain unchanged.

Configure the concrete Nuclei-based device SPI peripheral for ADS1299 mode 1 (CPOL=0, CPHA=1), beginning around 2-4 MHz. Map CS, RESET, PWDN, START and falling-edge DRDY through the existing callback port. Validate `ID -> internal test -> input short -> 250 SPS EEG` first.

For ADS1299-8, read one complete 27-byte RDATAC frame per DRDY and place it in a bounded ring buffer before application work.

Nuclei cores span many SoCs, so keep interrupt controller, DMA, cache and memory placement device-specific. First prove blocking SPI; then use DMA/non-blocking transfers where supported. On multicore/accelerated systems, isolate acquisition from AI/network workloads. For 64-channel low-skew capture, prefer the FPGA front end.

See `../DIRECT_USE.md` and `../_shared/other_ads1299_ready_app.[ch]`.
