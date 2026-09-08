# Sipeed Maix RISC-V ADS1299 Ready-to-Run Guide

This file is additive; existing target files remain unchanged.

Configure the selected SPI controller for ADS1299 mode 1 (CPOL=0, CPHA=1), start around 2-4 MHz, and map CS/RESET/PWDN/START plus falling-edge DRDY through the existing callbacks. Validate `ID -> internal test -> input short -> 250 SPS EEG` before AI/application workloads are enabled.

For ADS1299-8, service one complete 27-byte frame per DRDY and move it into a bounded queue before vision/AI/network processing.

Maix platforms commonly include RISC-V compute/accelerator workloads. Keep acquisition isolated from camera, KPU/AI and display tasks. First prove blocking SPI, then add DMA/non-blocking transfers. For 64-channel low-skew EEG, use a deterministic FPGA capture front end and use the Maix device for processing/visualization/transport.

See `../DIRECT_USE.md` and `../_shared/other_ads1299_ready_app.[ch]`.
