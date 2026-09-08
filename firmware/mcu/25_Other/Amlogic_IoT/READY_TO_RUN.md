# Amlogic IoT ADS1299 Ready-to-Run Guide

This file is additive; existing target files remain unchanged.

Configure the selected SPI controller as ADS1299 mode 1 (CPOL=0, CPHA=1), initially around 2-4 MHz. Map CS, RESET, PWDN, START and falling-edge DRDY through the existing callbacks. Validate `ID -> internal test -> input short -> 250 SPS EEG` before normal use.

For ADS1299-8, service one complete 27-byte RDATAC frame for each DRDY and enqueue it before network/application work.

Amlogic IoT targets may combine connectivity and application processing. Keep network, storage and media tasks outside the timing-critical acquisition path and use bounded queues between acquisition and application threads/cores. First prove blocking SPI, then use DMA/non-blocking transfers where supported. For 64-channel systems, prefer deterministic multi-lane or FPGA capture.

See `../DIRECT_USE.md` and `../_shared/other_ads1299_ready_app.[ch]`.
