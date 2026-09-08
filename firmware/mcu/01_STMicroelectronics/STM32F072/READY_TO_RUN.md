# STM32F072 ADS1299 ready-to-run path

Use this folder's existing board/port integration together with `../_shared/stm32_ads1299_ready_app.*` and `firmware/core_driver/ads1299/ads1299_profiles.*`.

Recommended bring-up: configure SPI mode 1, GPIO control lines and DRDY falling-edge EXTI; initialize the core driver; read ID; validate the internal test signal; validate input-short noise; then enable the EEG profile.

For USB-capable F0 applications, keep acquisition and USB transmission decoupled with a ring buffer so USB service latency cannot block DRDY servicing.

Do not perform the full 27-byte SPI transaction inside the EXTI callback. See `../DIRECT_USE.md` for the full flow.
