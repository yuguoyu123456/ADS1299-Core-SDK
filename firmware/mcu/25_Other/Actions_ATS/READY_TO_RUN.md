# Actions ATS ADS1299 Ready-to-Run Guide

This file is additive; existing target files remain unchanged.

Use the concrete ATS device SPI as ADS1299 mode 1 (CPOL=0, CPHA=1), initially around 2-4 MHz. Map CS, RESET, PWDN, START and falling-edge DRDY through the existing callbacks. Validate `ID -> internal test -> input short -> 250 SPS EEG` before normal acquisition.

Read exactly one 27-byte ADS1299-8 RDATAC frame per DRDY and push it to a bounded queue before audio/application work.

Actions ATS devices are often used in multimedia/audio systems. Keep audio codecs, media pipelines and storage completely outside the DRDY path. First prove blocking SPI, then use DMA/non-blocking transfers where the concrete SoC SDK supports them. For 64-channel capture, use dedicated deterministic lanes or the repository FPGA front end rather than sharing timing-critical acquisition with media workloads.

See `../DIRECT_USE.md` and `../_shared/other_ads1299_ready_app.[ch]`.
