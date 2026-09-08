# ATmega328 ADS1299 Ready-to-Run Guide

This file is additive; existing target files remain unchanged.

Use hardware SPI in ADS1299 mode 1 (CPOL=0, CPHA=1), starting conservatively. Map CS/RESET/PWDN/START and falling-edge DRDY through the existing callbacks. Validate `ID -> internal test -> input short -> 250 SPS EEG` before electrodes.

For ADS1299-8, one RDATAC frame is 27 bytes. Keep the DRDY ISR extremely short and use a small fixed-size buffer. Prefer forwarding raw 24-bit data rather than floating-point conversion on-device.

ATmega328 RAM and CPU resources are limited. This target is intended for simple single-ADS1299 experiments at conservative rates, not 8 x ADS1299 / 64-channel aggregation. Avoid dynamic allocation, large queues and verbose logging during acquisition.

See `../DIRECT_USE.md` and `../_shared/other_ads1299_ready_app.[ch]`.
