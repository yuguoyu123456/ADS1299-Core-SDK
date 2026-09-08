# STM32H723 ADS1299 ready-to-run path

Reuse this folder's existing port/board/Cube integration and add `../_shared/stm32_ads1299_ready_app.*` plus the ADS1299 profile layer.

Bring-up sequence: SPI mode 1 -> GPIO control -> DRDY EXTI -> core-driver init -> ID read -> internal-test profile -> input-short profile -> EEG profile.

For high-throughput H7 designs, use SPI DMA only after the simple blocking path is proven. Keep cache/DMA buffer placement coherent with your STM32H7 memory/cache configuration, and isolate transport from acquisition with a ring buffer.

For 8 x ADS1299 / 64 channels, use multiple SPI/DMA lanes or FPGA capture when tight inter-device alignment is required.

See `../DIRECT_USE.md` for the complete workflow.
