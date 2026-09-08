# STM32H563 ADS1299 ready-to-run path

Reuse this model folder's existing ADS1299 adapter/board integration and add `../_shared/stm32_ads1299_ready_app.*` plus the core-driver profile layer.

Recommended validation: SPI mode 1 and GPIO control -> DRDY EXTI -> `ads1299_init()` -> ID read -> internal test -> input short -> EEG profile. Keep the DRDY interrupt short and perform the exact 27-byte frame transfer in a dedicated acquisition context.

For sustained streaming, use SPI DMA and a ring buffer between acquisition and USB/Ethernet/storage. Track frame sequence and overflow counters; never silently overwrite unread data.

For 8 x ADS1299, multiple SPI/DMA lanes or FPGA-assisted capture provide better timing isolation than a single heavily shared bus.

See `../DIRECT_USE.md` for the complete flow.
