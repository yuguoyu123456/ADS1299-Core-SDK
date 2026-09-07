# Official sources inspected — 2026-09-06

- [ST CMSIS device U5](https://github.com/STMicroelectronics/cmsis-device-u5/tree/624374fa1e21ca195d6f2102ac0caaa50d0ea4c8),
  624374fa1e21ca195d6f2102ac0caaa50d0ea4c8: STM32U575xx register definitions.
  Apache-2.0; notices retained unchanged in the external checkout.
- [ST CubeU5](https://github.com/STMicroelectronics/STM32CubeU5/tree/c2846b9581183800cfec48c7e3fb7409aae78cf6),
  c2846b9581183800cfec48c7e3fb7409aae78cf6: NUCLEO-U575ZI-Q polling-master
  example and separate TrustZoneDisabled template. Example MCU signals are
  PA5 SCLK / PA6 MISO / PA7 MOSI, AF5, SPI1 kernel SYSCLK. This is not a confirmed
  connector map. Latest linker requires newer ld READONLY syntax; actual local
  link instead uses the unchanged official v1.1.0 template, commit
  133d940e5bd3a38955b9ba0cdf316f9681d0af2d. Device source and linker are separately
  pinned; this does not claim a full CubeU5 package build.
- [ST U5 HAL](https://github.com/STMicroelectronics/stm32u5xx-hal-driver/tree/1fe66fe15d7c6fd6ee8f4390c7dc6c48c6224a35),
  1fe66fe15d7c6fd6ee8f4390c7dc6c48c6224a35: inspected TransmitReceive,
  SPI_CloseTransfer and SPI1 clock definitions. BSD-3-Clause; license retained.
  HAL is a read-only API/sequence reference; no HAL code is copied or linked into
  the original adapter. The SPI implementation differs from STM32G4.

Downloads are confined to this platform's ignored tests/.tools. Cube components
have separate licenses: consult the selected resource's notices before any
redistribution. No vendor resource has been relicensed as original project code.

[ST ES0499 device errata](https://www.st.com/resource/en/errata_sheet/es0499-stm32u575xx-and-stm32u585xx-device-errata-stmicroelectronics.pdf)
identifies output truncation when disabling immediately after EOT. The adapter
adds a calculated delay; two actual SCK periods is our conservative implementation
choice, not a numeric guarantee supplied by ST. Physical edge timing remains unverified.
