# Platform support matrix

ADS1299-Core-SDK separates **project presence** from **verified support**. The MCU catalog currently contains **30 representative MCU/DSP projects**.

## Compiles — clean build verified

| Vendor | Project | Toolchain | Hardware |
|---|---|---|---|
| STMicroelectronics | `firmware/mcu/st/stm32f407_black/` | STM32Cube HAL / PlatformIO | Pre-hardware-validation |
| Espressif | `firmware/mcu/espressif/esp32s3_devkitc/` | ESP-IDF | Pre-hardware-validation |
| GigaDevice | `firmware/mcu/gigadevice/gd32f450z_eval/` | pinned GD32 PlatformIO + GD32F4xx SPL | Pre-hardware-validation |
| Nordic Semiconductor | `firmware/mcu/nordic/nrf52840_dk/` | nRF Connect SDK v3.4.0 LTS / Zephyr | Pre-hardware-validation |
| Nordic Semiconductor | `firmware/mcu/nordic/nrf5340_dk/` | nRF Connect SDK v3.4.0 LTS / Zephyr | Pre-hardware-validation |
| Raspberry Pi | `firmware/mcu/raspberry_pi/rp2040_pico/` | Pico SDK 2.3.0 | Pre-hardware-validation |
| Raspberry Pi | `firmware/mcu/raspberry_pi/rp2350_pico2/` | Pico SDK 2.3.0 | Pre-hardware-validation |
| WCH | `firmware/mcu/wch/ch32v307_evt/` | WCH NoneOS / PlatformIO | Pre-hardware-validation |

## Reference source / build-pending

The following are **not yet verified support**. Each has a target manifest, README, shared ADS1299 reference-app path and board-SDK contract, but must receive a real vendor-SDK BSP and clean build before status is upgraded.

| Vendor/family | Project |
|---|---|
| ST STM32H7 | `st/stm32h743_nucleo` |
| Espressif ESP32-C6 | `espressif/esp32c6_devkitc` |
| NXP i.MX RT | `nxp/mimxrt1060_evk` |
| NXP LPC55 | `nxp/lpc55s69_evk` |
| Renesas RA6 | `renesas/ek_ra6m5` |
| Renesas RX72N | `renesas/rx72n_envision` |
| TI MSPM0 | `ti/mspm0g3507_launchpad` |
| TI C2000 DSP | `ti/c2000_f28379d_launchpad` |
| TI TM4C129x | `ti/tm4c1294_launchpad` |
| Infineon PSoC 6 | `infineon/cy8ckit_062s2_43012` |
| Silicon Labs EFR32MG24 | `silicon_labs/efr32mg24_explorer` |
| Microchip SAME54 | `microchip/same54_xplained_pro` |
| Microchip SAMD51 | `microchip/samd51_xplained_pro` |
| Microchip megaAVR | `microchip/atmega4809_curiosity_nano` |
| Nuvoton M480 | `nuvoton/numaker_m487` |
| HDSC HC32F460 | `hdsc/hc32f460_eval` |
| Artery AT32F435 | `artery/at32f435_start` |
| MindMotion MM32F3277 | `mindmotion/mm32f3277_eval` |
| Nationstech N32G455 | `nationstech/n32g455_eval` |
| Puya PY32F403 | `puya/py32f403_eval` |
| Geehy APM32F407 | `geehy/apm32f407_mini` |
| STC STC32G12 | `stc/stc32g12k128_dev` |

## Status vocabulary

- **Reference source / build-pending** — concrete source profile and BSP contract exist, but no clean vendor-toolchain build evidence yet.
- **Compiles** — clean build verified with the documented toolchain.
- **Bench-tested** — real ADS1299 hardware verified.
- **24h-tested** — long-duration acquisition verified.

## Minimum acceptance before `Compiles`

1. Exact board/MCU and SDK/toolchain are pinned or documented.
2. Build system creates an executable from a clean environment.
3. ADS1299 SPI Mode 1 semantics are preserved.
4. RESET, START, PWDN and DRDY are wired through the portable port contract.
5. Device ID and register access paths compile against the common core.
6. Internal-test configuration and frame decode compile against the common core.
7. Common packet output is linked.
8. CI or reproducible local clean-build evidence is recorded.

Hardware evidence is still required separately before `Bench-tested`.
