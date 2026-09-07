# Official references and provenance — 2026-09-06

- [ST CMSIS device G4](https://github.com/STMicroelectronics/cmsis-device-g4/tree/626ee412334a5ed2e5b320af5a8d77d69f03a558),
  commit 626ee412334a5ed2e5b320af5a8d77d69f03a558: exact STM32G474 register
  definitions, genuine system source and GCC startup. Apache-2.0; original notices
  retained in the ignored dependency checkout.
- [ST CubeG4 v1.5.2](https://github.com/STMicroelectronics/STM32CubeG4/tree/v1.5.2),
  commit fd05229e8a53bdec8478cb0760219c8789e947b0: NUCLEO-G474RE Templates
  STM32CubeIDE/STM32G474RETX_FLASH.ld, unchanged external BSD-3-Clause resource.
  Polling-master SPI example confirms PB3/PB4/PB5 AF5 routing at MCU level.
  Its application/MSP code is not linked or copied into our adapter.

The latest Cube tree 140db648b016ba49b05ca790b5b66e3477807839 linker uses syntax
rejected by local GCC 9.2.1 ld (.ARM.extab non-constant expression). The reference
pins the actual older official linker rather than modifying vendor files or
claiming the latest toolchain passed. Device headers and linker are separately
pinned; this is not a claim to build the entire CubeG4 package.

CMSIS Core headers come from the existing installed STM32CubeH7 package. All
downloads remain in this platform's ignored tests/.tools directory. No full HAL,
SDK, startup or linker files enter shipping adapter sources. Port implementation
is original; all ADS1299 register definitions remain exclusively in shared Core.
