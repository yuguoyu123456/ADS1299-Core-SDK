# STM32L552 ADS1299 ready-to-run path

Reuse this model folder's existing ADS1299 board/port integration and add `../_shared/stm32_ads1299_ready_app.*` plus the core-driver profile layer.

Validate in order: SPI mode 1 -> GPIO control -> DRDY falling-edge EXTI -> ID read -> internal test -> input short -> EEG profile. Keep the exact 27-byte transfer outside the EXTI callback.

If TrustZone or security partitioning is used, keep SPI/GPIO ownership and acquisition buffers consistently assigned to the intended security domain. Add DMA only after the basic path is stable and keep transport behind a ring buffer.

See `../DIRECT_USE.md` for the complete workflow.
