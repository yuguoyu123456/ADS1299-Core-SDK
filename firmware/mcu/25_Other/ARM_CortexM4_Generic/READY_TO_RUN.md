# Generic Cortex-M4 ADS1299 Ready-to-Run Guide

Additive guide; existing files remain unchanged.

Configure SPI mode 1 (CPOL=0, CPHA=1), start around 2-4 MHz, and map CS/RESET/PWDN/START plus falling-edge DRDY through the existing `ads1299_port`. Validate `ID -> internal test -> input short -> 250 SPS EEG` first.

ADS1299-8 produces a 27-byte RDATAC frame per DRDY. Use `DRDY -> high-priority acquisition event -> SPI/DMA read -> header/status validation -> ring buffer -> transport` and keep floating-point/DSP work outside the ISR.

Cortex-M4 is a good single-device acquisition class. After blocking-path validation, use DMA and ping-pong/ring buffering on MCUs that provide it. DSP/FPU capabilities may process buffered data but must not delay acquisition. For 64 channels, multiple SPI/DMA lanes or the FPGA capture front end are preferred.

See `../DIRECT_USE.md` and `../_shared/other_ads1299_ready_app.[ch]`.
