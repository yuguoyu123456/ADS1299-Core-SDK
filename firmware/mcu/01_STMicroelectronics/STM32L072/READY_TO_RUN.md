# STM32L072 ADS1299 ready-to-run path

Reuse this folder's existing ADS1299 port/board integration and add `../_shared/stm32_ads1299_ready_app.*` plus the core-driver profile layer.

Configure SPI mode 1, control GPIO and DRDY falling-edge EXTI. Validate ID -> internal test -> input short -> EEG profile before adding low-power behavior.

For low-power applications, remember that ADS1299 DRDY cadence defines when the MCU must wake and service SPI. Keep the acquisition window short, queue completed frames, then return to the selected low-power state only after transport buffering is safe.

Start at 250 SPS and one ADS1299 during bring-up. See `../DIRECT_USE.md` for the complete procedure.
