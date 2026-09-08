# STM32F303 ADS1299 ready-to-run path

Reuse this model folder's existing ADS1299 board/port adapter and add `../_shared/stm32_ads1299_ready_app.*` plus the core-driver profile layer.

Bring-up order: SPI mode 1 -> control GPIO -> DRDY falling-edge EXTI -> driver init -> ID read -> internal test -> input short -> EEG. Keep the 27-byte read outside the EXTI callback and push completed frames into a ring buffer.

The F303 timer/ADC resources are not required for ADS1299 acquisition; keep the ADS1299 timing anchored to DRDY. For multi-device systems, validate one ADS1299 first before enabling multiple chip-select paths.

See `../DIRECT_USE.md` for the full application sequence.
