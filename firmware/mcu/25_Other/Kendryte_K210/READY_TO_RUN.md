# Kendryte K210 ADS1299 Ready-to-Run Guide

This file is additive; existing target files remain unchanged.

Configure the selected SPI controller for ADS1299 mode 1 (CPOL=0, CPHA=1), beginning around 2-4 MHz. Map CS, RESET, PWDN, START and falling-edge DRDY through the existing callbacks. Validate `ID -> internal test -> input short -> 250 SPS EEG` before enabling AI/application workloads.

Read one complete 27-byte ADS1299-8 RDATAC frame per DRDY and enqueue it into a bounded buffer.

K210 is dual-core and often runs compute-heavy AI tasks. Keep one high-priority acquisition context isolated from KPU/AI processing and move completed frames between contexts through bounded queues. First prove blocking SPI, then use DMA if supported by the selected SDK. For 64-channel systems, an FPGA capture front end feeding K210 is preferred for deterministic alignment.

See `../DIRECT_USE.md` and `../_shared/other_ads1299_ready_app.[ch]`.
