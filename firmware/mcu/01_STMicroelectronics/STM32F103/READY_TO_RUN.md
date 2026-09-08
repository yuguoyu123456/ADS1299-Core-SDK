# STM32F103 ADS1299 ready-to-run path

Use this folder's existing board/port integration together with `../_shared/stm32_ads1299_ready_app.*` and `firmware/core_driver/ads1299/ads1299_profiles.*`.

CubeMX/Cube HAL bring-up sequence: SPI master mode 1 -> CS/RESET/PWDN/START outputs -> DRDY falling-edge EXTI -> `ads1299_init()` -> ID read -> internal test -> input short -> normal EEG profile.

For F103 systems, prefer interrupt-driven DRDY plus a short acquisition routine and keep UART/USB/CAN transport behind a ring buffer. Do not block inside EXTI callbacks.

If multiple ADS1299 devices share one SPI peripheral, give each device its own CS and serialize accesses only after validating one-device acquisition first.

See `../DIRECT_USE.md` for the complete workflow.
