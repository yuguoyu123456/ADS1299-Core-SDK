# STM32C031 ADS1299 Port

Status: **TEMPLATE / integration-complete for this model; not BUILD-VERIFIED or BOARD-VERIFIED**. Hardware validation is not implied.

## Recommended reference board

Use **STMicroelectronics NUCLEO-C031C6 (MB1717)** as the primary reference board for this model folder. It carries an **STM32C031C6T6** MCU and an on-board ST-LINK debugger/programmer. The intended software environment is **STM32CubeC0 + STM32CubeIDE** (or another ST-supported toolchain using the same generated HAL project).

Official board page: https://www.st.com/en/evaluation-tools/nucleo-c031c6.html

Official board data brief: https://www.st.com/resource/en/data_brief/nucleo-c031c6.pdf

## What a beginner edits

For normal bring-up, do **not** edit `../../../core_driver/ads1299/`.

The board-specific edit point is:

- `board/board_config.h`

Use STM32CubeMX GPIO labels `ADS1299_CS`, `ADS1299_DRDY`, `ADS1299_RESET`, `ADS1299_PWDN`, and `ADS1299_START`. Configure the selected SPI peripheral as **SPI Mode 1 (CPOL=0, CPHA=1), MSB-first**, software-controlled CS. Start at a conservative SPI clock until ID read and self-test pass.

See `board/README.md` and `board/pinmap.md` before wiring. The exact NUCLEO header pins must be checked against the board revision and the CubeMX pin assignment you choose; this repository does not fabricate a fixed pin map where the application has not selected one.

## Repository layers

- Shared ADS1299 behavior: `../../../core_driver/ads1299/`
- Canonical packet code: `../../../common/data_packet/`
- STM32C031 board configuration: `board/`
- STM32 HAL adapter: `ads1299_port/`
- Runnable integration example: `examples/stm32c031_beginner_demo.c`
- STM32 example glue: `examples/stm32c031_example_platform.c`
- Legacy-compatible example entry: `examples/main_ads1299.c`
- Host/software smoke tests: `tests/`
- Cube project integration steps: `integration.md`

The platform layer never owns ADS1299 register definitions. ADS1299 behavior remains in the shared core.

## Beginner bring-up flow

1. Install STM32CubeIDE and STM32CubeC0.
2. Create a project for **NUCLEO-C031C6 / STM32C031C6T6**.
3. Configure one SPI peripheral as Mode 1, MSB-first, software NSS.
4. Configure GPIO labels for CS, DRDY, RESET, PWDN, and START.
5. Configure a UART if you want the included text/binary streaming example.
6. Add the shared ADS1299 core sources, STM32C031 port sources, board include path, and example sources listed in `integration.md`.
7. After CubeMX-generated peripheral initialization, call the beginner demo entry described in `examples/README.md`.
8. First verify device ID, then internal-test data, then input-short, then 250-SPS EEG acquisition, then streaming.

Expected text-stage progression is conceptually:

```text
ADS1299: port initialized
ADS1299: reset complete
ADS1299: ID read OK
ADS1299: internal test configured
ADS1299: input short configured
ADS1299: EEG 250 SPS configured
ADS1299: streaming
```

Exact numeric channel values depend on the actual ADS1299 hardware and wiring. Do not treat the text above as a hardware measurement record.

## ADS1299 invariants used by this model

- SPI Mode 1: CPOL=0, CPHA=1
- MSB-first
- ADS1299-8 continuous raw frame: 27 bytes
- Shared repository packet where used: 49 bytes, sync `0xA5 0x5A`, version/flags, sequence, timestamp, 3 status bytes, 8 int32 channels, CRC16

## Validation status

- Shared/software test infrastructure: present
- STM32C031 port host self-test: present
- STM32C031 model integration smoke-test entry: present
- STM32CubeC0 / CubeIDE reference-board build: **not yet verified in this repository**
- Physical NUCLEO-C031C6 + ADS1299 board test: **not verified**
- Electrical safety / EMC / medical / production readiness: **not claimed**

Until an actual documented CubeIDE build succeeds, treat this folder as **TEMPLATE / integration-complete**, not BUILD-VERIFIED.

## First troubleshooting checks

If ID read fails, check in this order: ADS1299 power/reference/clock, RESET/PWDN state, shared ground, SPI Mode 1, MSB-first, software CS timing, MISO/MOSI/SCK wiring, and the selected CubeMX SPI handle.

If ID succeeds but acquisition stalls, inspect DRDY polarity/wiring and confirm that no UART or other transport work is blocking the DRDY timing path.

For detailed integration and source ownership rules, continue with `integration.md`.

## Maintenance metadata

Catalog maintenance ID: **114**. This number is a repository maintenance ordering identifier, not a global sales ranking or supply guarantee.
