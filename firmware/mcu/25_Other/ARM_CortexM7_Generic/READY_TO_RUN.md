# Generic Cortex-M7 ADS1299 Ready-to-Run Guide

Additive guide; existing files remain unchanged.

Configure SPI mode 1 (CPOL=0, CPHA=1), start around 2-4 MHz, and map CS/RESET/PWDN/START plus falling-edge DRDY through the existing callbacks. Validate `ID -> internal test -> input short -> 250 SPS EEG` before increasing throughput.

For ADS1299-8, read one complete 27-byte RDATAC frame per DRDY. Use SPI DMA plus bounded ping-pong/ring buffers after the blocking path is proven. Keep DSP, networking and floating-point conversion outside the timing-critical ISR.

Cortex-M7 systems frequently include data cache and multiple SRAM regions. DMA buffers must be placed in DMA-accessible memory and maintained using the concrete MCU's cache-coherency rules. For 8 x ADS1299 / 64 channels, multiple SPI/DMA lanes are practical on capable devices; for the tightest inter-device skew use `firmware/fpga/common` as the deterministic capture front end.

See `../DIRECT_USE.md` and `../_shared/other_ads1299_ready_app.[ch]`.
