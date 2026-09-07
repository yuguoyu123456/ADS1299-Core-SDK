# Official sources and provenance

Only official STMicroelectronics and Texas Instruments material defines this
reference. No third-party driver source was copied.

| Scope | Source | Use |
|---|---|---|
| MCU | [STM32H743ZI product page](https://www.st.com/en/microcontrollers-microprocessors/stm32h743zi.html) and DS12110 Rev 10 | CPU, memory, SPI, DMA, USB and Ethernet capability |
| Board | [UM2407 Rev 6](https://www.st.com/resource/en/user_manual/um2407-stm32h7-nucleo144-board-stmicroelectronics.pdf) | MB1364 connector and solder-bridge mapping |
| Board CAD | [NUCLEO-H743ZI CAD resources](https://www.st.com/en/evaluation-tools/nucleo-h743zi.html#cad-resources) | MB1364-H743ZI-C01/E01 schematics |
| SDK | [STM32CubeH7](https://github.com/STMicroelectronics/STM32CubeH7) release 1.13.0 | HAL API and NUCLEO-H743ZI2 support baseline |
| AFE | [TI ADS1299 datasheet SBAS499C](https://www.ti.com/lit/ds/symlink/ads1299.pdf) | ADS1299 timing, commands, registers and SPI Mode 1 |

The official STM32CubeH7 repository is BSD-3-Clause. The SDK is installed by the
user/toolchain and is not redistributed in this repository. Repository-owned
adapter code is independently structured around the stable callback contract.
