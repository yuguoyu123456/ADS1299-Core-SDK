# STM32F446 ADS1299 ready-to-run path

Reuse this folder's existing ADS1299 adapter/board files and add `../_shared/stm32_ads1299_ready_app.*` plus the core-driver profile layer.

Configure SPI mode 1 and DRDY falling-edge EXTI. Validate in this order: ID read -> internal test -> input short -> EEG profile. Keep EXTI short and perform the 27-byte frame read in the acquisition context.

F446 projects can move to SPI DMA once the polling/interrupt path is proven. Use a ring buffer between acquisition and USB/UART/CAN/other transport so transport latency never causes missed DRDY events.

See `../DIRECT_USE.md` for the full workflow and multi-device strategy.
