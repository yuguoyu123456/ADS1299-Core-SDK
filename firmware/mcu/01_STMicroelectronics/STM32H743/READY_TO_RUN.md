# STM32H743 ADS1299 ready-to-run path

Reuse this folder's existing ADS1299 adapter/board integration and add `../_shared/stm32_ads1299_ready_app.*` plus the core-driver profile layer.

Validate the hardware in order: SPI mode 1 -> control GPIO -> DRDY EXTI -> ID read -> internal test -> input short -> EEG profile. The EXTI callback should only signal acquisition; perform the 27-byte SPI transfer outside the ISR.

For DMA on STM32H743, place acquisition buffers in DMA-accessible memory and handle D-cache coherency correctly. Use a ring buffer between DMA completion and USB/Ethernet/storage tasks, with explicit overflow accounting.

H743 has enough system resources to coordinate several ADS1299 devices, but FPGA-assisted parallel capture remains preferable for minimum skew across eight devices.

See `../DIRECT_USE.md` for the full flow.
