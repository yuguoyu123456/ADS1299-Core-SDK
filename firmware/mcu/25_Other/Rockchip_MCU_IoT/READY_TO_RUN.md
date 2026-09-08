# Rockchip MCU/IoT ADS1299 Ready-to-Run Guide

This file is additive; existing target files remain unchanged.

Configure the real-time MCU/IoT SPI controller for ADS1299 mode 1 (CPOL=0, CPHA=1), beginning around 2-4 MHz. Map CS, RESET, PWDN, START and falling-edge DRDY through the existing port callbacks. Validate `ID -> internal test -> input short -> 250 SPS EEG` before application workloads.

Read exactly one 27-byte ADS1299-8 RDATAC frame per DRDY and pass completed frames through a bounded queue/shared-memory channel.

When a Rockchip system combines MCU and Linux/application cores, reserve the real-time MCU context for acquisition and keep Linux/network/storage work on the application side. Avoid blocking inter-core communication in the DRDY path. For 64-channel systems, use deterministic FPGA or multi-lane capture and use Rockchip for control/compute/transport.

See `../DIRECT_USE.md` and `../_shared/other_ads1299_ready_app.[ch]`.
