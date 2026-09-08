# STM32L476 ADS1299 ready-to-run path

Reuse this folder's existing ADS1299 adapter/board integration and add `../_shared/stm32_ads1299_ready_app.*` plus the core-driver profile layer.

Bring-up order: SPI mode 1 -> control GPIO -> DRDY EXTI -> driver init -> ID read -> internal test -> input short -> EEG profile. Keep the DRDY ISR short and execute the 27-byte SPI transfer in the acquisition context.

For sustained logging, use DMA/ring buffers after the basic path is stable. Low-power transitions must not delay DRDY service or corrupt SPI state.

See `../DIRECT_USE.md` for the complete workflow and multi-device guidance.
