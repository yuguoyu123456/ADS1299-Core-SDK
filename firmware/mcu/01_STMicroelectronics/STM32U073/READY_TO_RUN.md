# STM32U073 ADS1299 ready-to-run path

Reuse this folder's existing ADS1299 adapter/board integration and add `../_shared/stm32_ads1299_ready_app.*` plus the core-driver profile layer.

Configure SPI mode 1, control GPIO and DRDY falling-edge EXTI. Validate ID -> internal test -> input short -> EEG profile before enabling low-power policies.

Keep the DRDY ISR short. Complete the 27-byte frame transfer in the acquisition context, queue the frame, and only then allow low-power transitions. Maintain an overflow counter so delayed wakeups are detectable.

See `../DIRECT_USE.md` for the complete workflow.
