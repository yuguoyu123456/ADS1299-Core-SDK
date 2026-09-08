# STM32H750 ADS1299 ready-to-run path

Reuse this folder's existing ADS1299 board/port/Cube integration and add `../_shared/stm32_ads1299_ready_app.*` plus the core-driver profile layer.

Bring-up order: SPI mode 1 -> GPIO control -> DRDY EXTI -> driver init -> ID read -> internal test -> input short -> EEG profile. Keep the exact 27-byte transfer outside the EXTI callback.

For DMA-based acquisition, use DMA-accessible buffers and handle H7 cache coherency correctly. Keep transport/storage separate from the acquisition path and record queue overflow explicitly.

For 8 x ADS1299 systems, validate one device first, then add multiple SPI/DMA lanes or FPGA capture for tighter synchronization.

See `../DIRECT_USE.md` for the complete workflow.
