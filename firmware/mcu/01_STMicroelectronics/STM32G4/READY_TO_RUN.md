# STM32G4 ADS1299 ready-to-run path

Reuse this folder's existing model-specific adapter/board integration and add `../_shared/stm32_ads1299_ready_app.*` plus the ADS1299 core-driver profiles.

Configure SPI mode 1 and DRDY falling-edge EXTI. Validate ID -> internal test -> input short -> EEG profile before enabling application DSP or transport.

STM32G4-class projects can use SPI DMA and timer/DSP resources, but ADS1299 sample timing must stay anchored to DRDY. Keep acquisition, processing and transport in separate stages with a ring buffer and explicit overflow counter.

For multiple ADS1299 devices, validate one-device timing first, then add per-device CS and sample-alignment checks.

See `../DIRECT_USE.md` for the complete integration flow.
