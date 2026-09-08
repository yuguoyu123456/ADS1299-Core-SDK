# STM32N657 ADS1299 ready-to-run path

Reuse this folder's existing ADS1299 board/port integration and add `../_shared/stm32_ads1299_ready_app.*` plus the core-driver profile layer.

Bring-up remains identical regardless of application-AI workload: SPI mode 1 -> control GPIO -> DRDY EXTI -> driver init -> ID read -> internal test -> input short -> EEG profile.

Keep ADS1299 acquisition isolated from high-latency AI/vision workloads. Use DMA and a ring buffer after the basic path is validated, and give acquisition higher scheduling priority than inference/transport tasks.

For 8 x ADS1299, FPGA-assisted parallel capture is recommended when minimum inter-device skew matters.

See `../DIRECT_USE.md` for the complete workflow.
