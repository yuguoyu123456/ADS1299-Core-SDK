# Samsung ARTIK ADS1299 Ready-to-Run Guide

This file is additive; existing target files remain unchanged.

Configure the target SPI controller as ADS1299 mode 1 (CPOL=0, CPHA=1), start around 2-4 MHz, and map CS/RESET/PWDN/START plus falling-edge DRDY through the existing callbacks. Validate `ID -> internal test -> input short -> 250 SPS EEG` first.

For ADS1299-8, service one complete 27-byte frame per DRDY and enqueue it into a bounded acquisition queue before Linux/network/application processing.

ARTIK-class systems may run rich OS/network stacks. Keep acquisition on the most deterministic execution context available and isolate it from cloud, Wi-Fi, storage and UI work. If Linux is used, prefer a dedicated MCU/FPGA capture front end for hard timing. For 64-channel systems, FPGA capture plus ARTIK-class compute/transport is the preferred architecture.

See `../DIRECT_USE.md` and `../_shared/other_ads1299_ready_app.[ch]`.
