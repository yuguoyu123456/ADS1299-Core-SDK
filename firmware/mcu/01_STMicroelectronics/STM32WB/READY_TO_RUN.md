# STM32WB ADS1299 ready-to-run path

Reuse this folder's existing ADS1299 board/port integration and add `../_shared/stm32_ads1299_ready_app.*` plus the core-driver profile layer.

Validate SPI mode 1 -> control GPIO -> DRDY EXTI -> ID read -> internal test -> input short -> EEG profile before enabling BLE traffic.

Keep radio/BLE processing independent from the DRDY path. Use a ring buffer between ADS1299 acquisition and BLE packetization because connection events and RF scheduling can introduce variable latency.

For bandwidth-limited wireless streaming, preserve raw sample sequence numbers so packet loss can be detected on the receiver.

See `../DIRECT_USE.md` for the complete workflow.
