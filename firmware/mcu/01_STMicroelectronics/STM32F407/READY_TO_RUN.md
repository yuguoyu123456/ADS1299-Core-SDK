# STM32F407 ADS1299 ready-to-run path

This folder already contains a Cube HAL adapter. Keep it and add the shared application layer from `../_shared/stm32_ads1299_ready_app.*` plus `firmware/core_driver/ads1299/ads1299_profiles.*`.

Recommended flow: initialize the existing STM32Cube adapter -> `ads1299_init()` -> read ID -> internal-test profile -> input-short profile -> EEG profile -> DRDY-driven frame loop.

For sustained acquisition, use EXTI only to signal work; perform the 27-byte SPI read in a high-priority task/main-loop context. F407 projects can use SPI DMA and a ring buffer once polling/interrupt bring-up is stable, but validate the non-DMA path first.

For USB/Ethernet output, never transmit directly inside the DRDY path. Queue completed frames and service transport separately.

See `../DIRECT_USE.md` for 64-channel scaling guidance.
