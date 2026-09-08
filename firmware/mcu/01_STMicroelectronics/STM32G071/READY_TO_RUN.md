# STM32G071 ADS1299 ready-to-run path

Reuse this folder's existing ADS1299 board/port adapter and add `../_shared/stm32_ads1299_ready_app.*` plus the core-driver profile layer.

Bring-up order: SPI mode 1 -> control GPIO -> DRDY falling-edge EXTI -> `ads1299_init()` -> ID read -> internal test -> input short -> EEG profile.

Keep the DRDY ISR minimal and perform the 27-byte transfer in the acquisition context. Use a small ring buffer if UART/USB/other transport is slower than acquisition.

Start with one ADS1299 and 250 SPS before enabling multiple devices or higher rates. Board pin assignments must come from the actual schematic/CubeMX project.

See `../DIRECT_USE.md` for the complete workflow.
