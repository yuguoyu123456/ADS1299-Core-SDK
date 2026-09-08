# Generic Cortex-M0+ ADS1299 Ready-to-Run Guide

Additive guide; all existing files remain unchanged.

Use SPI mode 1 (CPOL=0, CPHA=1), start around 2-4 MHz, and map CS/RESET/PWDN/START plus falling-edge DRDY through the existing `ads1299_port`. Validate `ID -> internal test -> input short -> 250 SPS EEG` before normal electrodes.

An ADS1299-8 RDATAC frame is 27 bytes. Keep DRDY ISR minimal: signal acquisition, read one complete frame, validate status/header, then enqueue into a fixed-size ring buffer.

Cortex-M0+ targets are resource constrained: prefer raw 24-bit data, bounded static buffers, no floating-point work in the ISR, and conservative sample rates. Prove blocking SPI first; add DMA only if the actual MCU provides it. Do not treat generic M0+ as a direct 64-channel aggregator; use a stronger MCU or FPGA capture front end for 8 x ADS1299.

See `../DIRECT_USE.md` and `../_shared/other_ads1299_ready_app.[ch]`.
