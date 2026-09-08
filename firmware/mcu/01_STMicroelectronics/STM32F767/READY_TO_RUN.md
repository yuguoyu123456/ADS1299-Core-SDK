# STM32F767 ADS1299 ready-to-run path

Use the existing model-specific port/board integration in this folder and add `../_shared/stm32_ads1299_ready_app.*` plus the core-driver profile layer.

Recommended sequence: SPI mode 1 -> GPIO control -> DRDY EXTI -> driver init -> ID read -> internal test -> input short -> EEG. Keep the DRDY ISR minimal and launch/schedule a 27-byte SPI read outside the ISR.

F767-class devices are strong candidates for SPI DMA plus USB/Ethernet streaming. Keep DMA completion and transport queues independent, with an explicit frame counter and overflow counter.

For 8 x ADS1299, consider multiple SPI/DMA lanes or FPGA-assisted capture if all devices must be tightly aligned.

See `../DIRECT_USE.md` for the full integration flow.
