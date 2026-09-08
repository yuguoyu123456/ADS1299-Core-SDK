# STM32U5 ADS1299 ready-to-run path

Reuse this folder's existing ADS1299 port/board integration and add `../_shared/stm32_ads1299_ready_app.*` plus the core-driver profile layer.

Bring-up order: SPI mode 1 -> GPIO control -> DRDY EXTI -> `ads1299_init()` -> ID read -> internal test -> input short -> EEG profile. Keep the 27-byte transfer outside the EXTI callback.

For U5 low-power and security features, keep acquisition buffers and peripheral ownership consistent across power/security domains. Add SPI DMA after the basic path is proven and isolate transport with a ring buffer.

See `../DIRECT_USE.md` for the full workflow and 64-channel guidance.
