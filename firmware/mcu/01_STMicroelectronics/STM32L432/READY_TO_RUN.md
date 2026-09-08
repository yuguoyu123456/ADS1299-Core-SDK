# STM32L432 ADS1299 ready-to-run path

Reuse this folder's existing ADS1299 port/board integration and add `../_shared/stm32_ads1299_ready_app.*` plus the core-driver profile layer.

Configure SPI mode 1 and DRDY falling-edge EXTI. Validate ID -> internal test -> input short -> EEG profile. Keep the 27-byte frame transfer outside the EXTI callback and queue completed frames for transport.

L432 projects can combine low-power operation with DMA, but first prove the simple interrupt-driven path. Track frame sequence and overflow counters so missed data is visible.

See `../DIRECT_USE.md` for the complete integration flow.
