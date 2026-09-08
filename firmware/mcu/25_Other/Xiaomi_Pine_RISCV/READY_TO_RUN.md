# Xiaomi Pine RISC-V ADS1299 Ready-to-Run Guide

This file is additive; existing target files remain unchanged.

Configure the concrete Pine/RISC-V SPI peripheral for ADS1299 mode 1 (CPOL=0, CPHA=1), initially around 2-4 MHz. Route CS, RESET, PWDN, START and falling-edge DRDY through the existing callback port. Validate `ID -> internal test -> input short -> 250 SPS EEG` before enabling wireless/application workloads.

For ADS1299-8, read exactly one complete 27-byte RDATAC frame per DRDY and place it into a bounded ring buffer.

Keep wireless/network/application processing outside the DRDY service path. First prove blocking SPI with the ADS1299 internal test source, then add DMA/non-blocking transfer where the concrete SDK supports it. Track sequence gaps, SPI errors, late service and queue overflows. For 8 x ADS1299 / 64 channels, use a deterministic FPGA or stronger multi-lane acquisition controller and use the Pine RISC-V target for control/transport.

See `../DIRECT_USE.md` and `../_shared/other_ads1299_ready_app.[ch]`.
