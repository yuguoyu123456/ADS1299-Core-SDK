# MCU Reference Projects

`firmware/mcu/` is organized first by MCU vendor, then by exact board/MCU project. The repository now tracks **30 representative MCU/DSP projects** spanning major global ecosystems. Eight currently have clean CI build evidence; twenty-two have a concrete reference-source profile and BSP contract and remain `Reference source / build-pending` until a real vendor-toolchain clean build succeeds.

The catalog is deliberately broad, but it does **not** claim that all 30 platforms are verified support. Presence, compilation evidence and hardware evidence are separate concepts.

## Validation vocabulary

- **Compiles** — clean build verified with the documented toolchain.
- **Reference source / build-pending** — project directory, target manifest, shared ADS1299 application path and MCU BSP contract exist, but vendor-SDK build evidence is not complete.
- **Bench-tested** — exact board + ADS1299 hardware exercised.
- **24h-tested** — long-duration acquisition evidence exists.

## Compiles

- STMicroelectronics — `st/stm32f407_black/`
- Espressif — `espressif/esp32s3_devkitc/`
- GigaDevice — `gigadevice/gd32f450z_eval/`
- Nordic — `nordic/nrf52840_dk/`
- Nordic — `nordic/nrf5340_dk/`
- Raspberry Pi — `raspberry_pi/rp2040_pico/`
- Raspberry Pi — `raspberry_pi/rp2350_pico2/`
- WCH — `wch/ch32v307_evt/`

## Reference source / build-pending

- STMicroelectronics — `st/stm32h743_nucleo/` — STM32H7
- Espressif — `espressif/esp32c6_devkitc/` — ESP32-C6
- NXP — `nxp/mimxrt1060_evk/` — i.MX RT1062
- NXP — `nxp/lpc55s69_evk/` — LPC55S69
- Renesas — `renesas/ek_ra6m5/` — RA6M5
- Renesas — `renesas/rx72n_envision/` — RX72N
- Texas Instruments — `ti/mspm0g3507_launchpad/` — MSPM0G3507
- Texas Instruments — `ti/c2000_f28379d_launchpad/` — C2000 F28379D DSP
- Texas Instruments — `ti/tm4c1294_launchpad/` — TM4C1294
- Infineon/Cypress — `infineon/cy8ckit_062s2_43012/` — PSoC 6
- Silicon Labs — `silicon_labs/efr32mg24_explorer/` — EFR32MG24
- Microchip — `microchip/same54_xplained_pro/` — SAME54
- Microchip — `microchip/samd51_xplained_pro/` — SAMD51
- Microchip — `microchip/atmega4809_curiosity_nano/` — megaAVR 0-series
- Nuvoton — `nuvoton/numaker_m487/` — NuMicro M480
- HDSC — `hdsc/hc32f460_eval/` — HC32F460
- Artery — `artery/at32f435_start/` — AT32F435
- MindMotion — `mindmotion/mm32f3277_eval/` — MM32F3277
- Nationstech — `nationstech/n32g455_eval/` — N32G455
- Puya — `puya/py32f403_eval/` — PY32F403
- Geehy — `geehy/apm32f407_mini/` — APM32F407
- STC — `stc/stc32g12k128_dev/` — STC32G12

## Shared first-bring-up firmware

`common/reference_app/` contains the reusable ADS1299 acquisition path for build-pending projects. It owns reset, ID read, 250 SPS, gain 24, internal test signal, RDATAC/START, DRDY handling, frame decode and the common 49-byte packet. Each shared-style MCU project supplies only the MCU/board-specific BSP contract. This prevents 30 divergent copies of ADS1299 logic.

## Catalog policy

The framework target is now fixed at 30 representative projects. Future MCU work should normally **graduate existing projects** from `Reference source / build-pending` to `Compiles` rather than keep adding names. A platform may move to `Compiles` only after a documented clean build with its real toolchain/SDK.
