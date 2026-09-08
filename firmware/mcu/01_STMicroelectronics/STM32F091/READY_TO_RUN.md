# STM32F091 ADS1299 ready-to-run path

Use this folder's existing board/port integration together with `../_shared/stm32_ads1299_ready_app.*` and the core-driver profile layer.

Configure SPI mode 1, GPIO control lines, and DRDY falling-edge EXTI. Bring the device up with ID read -> internal test -> input short -> EEG profile. Keep the EXTI callback minimal and move the 27-byte SPI read to the acquisition context.

Use a ring buffer between acquisition and UART/USB/other transport so communication cannot block DRDY handling. Board pin numbers remain board-specific.

See `../DIRECT_USE.md` for the complete procedure.
