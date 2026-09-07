# Official sources and provenance

| Scope | Official source | Use |
|---|---|---|
| MCU | [STM32F407VG product page](https://www.st.com/en/microcontrollers-microprocessors/stm32f407vg.html) and DS8626 | CPU, memory, SPI, DMA, USB and Ethernet capability |
| Board | [UM1472 Rev 9](https://www.st.com/resource/en/user_manual/um1472-stm32f4-discovery-stmicroelectronics.pdf) | MB997 extension headers and on-board peripheral conflicts |
| Schematic | [MB997-F407VGT6-E01](https://www.st.com/resource/en/schematic_pack/mb997-f407vgt6-e01_schematic.pdf) | LIS3DSH SPI1 wiring and PE3 chip select |
| SDK | [STM32CubeF4](https://github.com/STMicroelectronics/STM32CubeF4) | HAL API and official STM32F4-Discovery baseline |
| AFE | [TI ADS1299 datasheet SBAS499C](https://www.ti.com/lit/ds/symlink/ads1299.pdf) | ADS1299 timing, commands, registers and SPI Mode 1 |

STM32CubeF4 HAL is BSD-3-Clause according to the official package license. It is
installed externally and is not redistributed here. The adapter was structured
independently around the repository callback contract.
