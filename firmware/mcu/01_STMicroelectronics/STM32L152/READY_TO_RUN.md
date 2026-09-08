# STM32L152 ADS1299 ready-to-run path

Reuse this model folder's existing ADS1299 adapter/board integration and add `../_shared/stm32_ads1299_ready_app.*` plus the core-driver profile layer.

Bring-up order: SPI mode 1 -> GPIO control -> DRDY falling-edge interrupt -> core-driver init -> ID read -> internal test -> input short -> EEG profile.

For low-power operation, wake on DRDY or a controlled acquisition schedule, complete the 27-byte transfer promptly, place the frame in a buffer, and keep communication work outside the DRDY interrupt path.

Validate one ADS1299 at 250 SPS before adding power-state transitions or multiple devices. See `../DIRECT_USE.md` for the complete workflow.
