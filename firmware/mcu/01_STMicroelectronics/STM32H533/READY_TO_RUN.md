# STM32H533 ADS1299 ready-to-run path

Reuse this folder's existing ADS1299 board/port integration and add `../_shared/stm32_ads1299_ready_app.*` plus the core-driver profile layer.

Bring-up sequence: SPI mode 1 -> GPIO control -> DRDY EXTI -> core-driver init -> ID read -> internal test -> input short -> EEG profile. Keep the 27-byte read outside the EXTI callback.

For high-rate or multi-device acquisition, use SPI DMA plus a dedicated high-priority acquisition task and a lock-free/ring-buffer handoff to transport. Maintain sample and overflow counters so data loss is observable.

When scaling to 8 x ADS1299, use multiple SPI/DMA lanes where practical or offload deterministic parallel capture to FPGA.

See `../DIRECT_USE.md` for the complete workflow.
