# MCU Reference Projects

`firmware/mcu/` is organized first by MCU vendor, then by exact board/MCU project. The repository currently tracks **23 representative MCU/DSP projects**. Eight have clean CI build evidence; fifteen have a concrete reference-source profile and BSP contract but still require vendor-SDK binding and clean-build evidence before they can be called `Compiles`.

## Validation vocabulary

- **Compiles** — clean build verified with the documented toolchain.
- **Reference source / build-pending** — project directory, target manifest, ADS1299 reference application path and MCU BSP contract exist, but vendor-SDK build evidence is not complete. This is not verified support.
- **Bench-tested** — exact board + ADS1299 hardware exercised.
- **24h-tested** — long-duration acquisition evidence exists.

## Projects

### Compiles

- STMicroelectronics — `st/stm32f407_black/`
- Espressif — `espressif/esp32s3_devkitc/`
- GigaDevice — `gigadevice/gd32f450z_eval/`
- Nordic — `nordic/nrf52840_dk/`
- Nordic — `nordic/nrf5340_dk/`
- Raspberry Pi — `raspberry_pi/rp2040_pico/`
- Raspberry Pi — `raspberry_pi/rp2350_pico2/`
- WCH — `wch/ch32v307_evt/`

### Reference source / build-pending

- NXP — `nxp/mimxrt1060_evk/`
- Renesas — `renesas/ek_ra6m5/`
- Texas Instruments MCU — `ti/mspm0g3507_launchpad/`
- Texas Instruments DSP — `ti/c2000_f28379d_launchpad/`
- Infineon/Cypress — `infineon/cy8ckit_062s2_43012/`
- Silicon Labs — `silicon_labs/efr32mg24_explorer/`
- Microchip 32-bit — `microchip/same54_xplained_pro/`
- HDSC — `hdsc/hc32f460_eval/`
- Artery — `artery/at32f435_start/`
- MindMotion — `mindmotion/mm32f3277_eval/`
- Nationstech — `nationstech/n32g455_eval/`
- Puya — `puya/py32f403_eval/`
- Geehy — `geehy/apm32f407_mini/`
- STC — `stc/stc32g12k128_dev/`
- Microchip AVR — `microchip/atmega4809_curiosity_nano/`

## Shared first-bring-up firmware

`common/reference_app/` contains the reusable ADS1299 acquisition path for build-pending projects. It owns reset, ID read, 250 SPS, gain 24, internal test signal, RDATAC/START, DRDY handling, frame decode and the common 49-byte packet. Each shared-style MCU project supplies only the board-specific BSP contract. This avoids divergent copies of ADS1299 logic.

The next maintenance objective is not to add more names. It is to graduate these fifteen projects one by one from `Reference source / build-pending` to `Compiles` using their real vendor SDKs and CI/toolchain evidence.
