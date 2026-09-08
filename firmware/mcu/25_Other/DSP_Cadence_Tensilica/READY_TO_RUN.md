# Cadence Tensilica DSP + ADS1299 Ready-to-Run Guide

This file is additive; existing target files remain unchanged.

Configure the concrete platform's SPI controller for ADS1299 mode 1 (CPOL=0, CPHA=1), initially around 2-4 MHz. Map CS, RESET, PWDN, START and falling-edge DRDY through the existing callbacks. Validate `ID -> internal test -> input short -> 250 SPS EEG` before enabling heavy DSP workloads.

Each ADS1299-8 RDATAC frame is 27 bytes. Use a bounded DMA/ring buffer and keep the DRDY path limited to timing, SPI transfer and frame enqueue.

Tensilica-based systems vary widely, so keep cache, DMA, interrupt routing and memory placement platform-specific. DSP/AI/audio workloads must consume completed buffers asynchronously and never hold the acquisition path. For 64-channel systems, FPGA capture plus DSP processing is the preferred low-skew architecture.

See `../DIRECT_USE.md` and `../_shared/other_ads1299_ready_app.[ch]`.
