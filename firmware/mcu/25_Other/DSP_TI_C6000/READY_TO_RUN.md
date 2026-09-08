# TI C6000 DSP + ADS1299 Ready-to-Run Guide

This file is additive; existing target files remain unchanged.

Configure the concrete platform SPI peripheral as ADS1299 mode 1 (CPOL=0, CPHA=1), start around 2-4 MHz for bring-up, and map CS/RESET/PWDN/START plus falling-edge DRDY through the existing callbacks. Validate `ID -> internal test -> input short -> 250 SPS EEG` first.

For ADS1299-8, read exactly one 27-byte frame per DRDY and move it into DMA-safe ping-pong/ring buffers before signal processing begins.

C6000 devices are appropriate for high-throughput EEG DSP, but real-time capture and DSP kernels should use separate buffers/tasks/interrupt priorities. Observe the concrete SoC's cache and DMA coherency rules and expose overrun/sequence-gap counters. For 8 x ADS1299 / 64 channels, deterministic FPGA capture feeding the DSP is preferred when cross-channel timing matters.

See `../DIRECT_USE.md` and `../_shared/other_ads1299_ready_app.[ch]`.
