# SiFive Freedom ADS1299 Ready-to-Run Guide

This file is additive; existing target files remain unchanged.

Configure the concrete SiFive platform SPI controller for ADS1299 mode 1 (CPOL=0, CPHA=1), beginning around 2-4 MHz. Route CS, RESET, PWDN, START and falling-edge DRDY through the existing callbacks. Validate `ID -> internal test -> input short -> 250 SPS EEG` first.

For ADS1299-8, service exactly one 27-byte RDATAC frame per DRDY and enqueue it into a bounded ring buffer.

SiFive Freedom platforms vary from microcontroller-class to richer SoCs. Keep PLIC/interrupt, DMA, cache and memory placement platform-specific. First prove blocking SPI, then use DMA/non-blocking transfers when available. For 8 x ADS1299 / 64 channels, use multi-lane capture only on verified capable hardware; otherwise use the FPGA front end.

See `../DIRECT_USE.md` and `../_shared/other_ads1299_ready_app.[ch]`.
