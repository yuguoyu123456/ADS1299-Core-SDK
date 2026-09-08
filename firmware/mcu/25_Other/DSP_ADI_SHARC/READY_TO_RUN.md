# ADI SHARC + ADS1299 Ready-to-Run Guide

This file is additive; existing target files remain unchanged.

Use the target SPI controller in ADS1299 mode 1 (CPOL=0, CPHA=1), begin around 2-4 MHz, and map CS/RESET/PWDN/START plus falling-edge DRDY through the existing port callbacks. Validate `ID -> internal test -> input short -> 250 SPS EEG` before DSP processing is enabled.

Read exactly one 27-byte ADS1299-8 RDATAC frame per DRDY and place it into a DMA-safe ping-pong/ring buffer.

SHARC is well suited for downstream EEG filtering/FFT/feature extraction, but DSP kernels must never block the acquisition service path. Use DMA and separate acquisition/processing buffers; expose sequence-gap and overrun counters. For 8 x ADS1299 / 64 channels, a deterministic FPGA capture front end feeding SHARC is preferred when channel alignment is important.

See `../DIRECT_USE.md` and `../_shared/other_ads1299_ready_app.[ch]`.
