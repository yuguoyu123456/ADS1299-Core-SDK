# STM32WL ADS1299 ready-to-run path

Reuse this folder's existing ADS1299 board/port integration and add `../_shared/stm32_ads1299_ready_app.*` plus the core-driver profile layer.

Bring-up order: SPI mode 1 -> control GPIO -> DRDY falling-edge EXTI -> driver init -> ID read -> internal test -> input short -> EEG profile.

Keep LoRa/sub-GHz radio work separate from ADS1299 acquisition. Radio transmission can be bursty and slow relative to EEG sampling, so queue frames in a ring buffer and include sample sequence numbers in the wireless protocol.

Do not transmit directly from the DRDY path. See `../DIRECT_USE.md` for the complete workflow.
