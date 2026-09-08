# STM32F429 ADS1299 ready-to-run path

Reuse the existing model-specific port/Cube adapter in this folder and add `../_shared/stm32_ads1299_ready_app.*` plus the core-driver profile layer.

Bring-up sequence: SPI mode 1 -> control GPIO -> DRDY EXTI -> `ads1299_init()` -> ID read -> internal test -> input short -> EEG profile. Keep transport separated from acquisition with a ring buffer.

F429-class systems are suitable for SPI DMA after the basic path is stable. Use DRDY to launch or schedule the 27-byte transfer; do not let display, SDRAM, USB, or Ethernet tasks block the acquisition path.

For multiple ADS1299 devices, validate frame alignment and sample counters before enabling all channels.

See `../DIRECT_USE.md` for the complete application and 64-channel guidance.
