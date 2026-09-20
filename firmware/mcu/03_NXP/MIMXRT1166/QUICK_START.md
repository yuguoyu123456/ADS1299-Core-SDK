# MIMXRT1166 ADS1299 Quick Start

Validation status: **TEMPLATE / NOT BUILD-VERIFIED / NOT BOARD-VERIFIED**.

## Reference board

Use the official **NXP MIMXRT1160-EVK** as the reference board for this leaf. NXP documents that board with a **MIMXRT1166DVM6A** device (600 MHz Cortex-M7 + 240 MHz Cortex-M4) and supports it in MCUXpresso SDK. Start from an official `evkmimxrt1160` MCUXpresso project rather than inventing startup, linker, clock, or pinmux files in this repository.

For the first ADS1299 bring-up, use one core/project only and keep the ADS1299 acquisition path independent of networking or other blocking middleware.

## The one board configuration point

Do not edit the shared ADS1299 core. Hardware-dependent values belong in the consuming MCUXpresso board project and feed:

`board/board_ads1299_config.h`

Define these macros from the pin/clock configuration that you actually generated or verified for your board revision:

- `ADS1299_BOARD_LPSPI_BASE`
- `ADS1299_BOARD_LPSPI_SOURCE_CLOCK_HZ`
- `ADS1299_BOARD_SPI_BAUD_HZ` (optional; defaults to 1 MHz)
- `ADS1299_BOARD_CS_GPIO`, `ADS1299_BOARD_CS_PIN`
- `ADS1299_BOARD_RESET_GPIO`, `ADS1299_BOARD_RESET_PIN`
- `ADS1299_BOARD_PWDN_GPIO`, `ADS1299_BOARD_PWDN_PIN`
- `ADS1299_BOARD_START_GPIO`, `ADS1299_BOARD_START_PIN`
- `ADS1299_BOARD_DRDY_GPIO`, `ADS1299_BOARD_DRDY_PIN`

The repository intentionally does not guess EVK header pins. Select free pins in MCUXpresso Config Tools, generate/verify the pinmux and clock configuration, then put only those board-specific definitions in the parent project.

## ADS1299 wiring contract

Connect the selected LPSPI signals to ADS1299 `SCLK`, `DIN` and `DOUT`. Connect software-controlled GPIOs to `CS`, `RESET`, `PWDN`, and `START`; connect `DRDY` to the selected input GPIO. Share the required digital ground and use the ADS1299 hardware design's verified power/reference circuitry.

Configure LPSPI as **SPI Mode 1 (CPOL=0, CPHA=1), MSB first**. Keep CS software-controlled. The default bring-up rate is 1 MHz; do not raise it until probe and self-test are reliable.

## Files to add to the official project

The leaf CMake integration consumes the shared ADS1299 core plus:

- `ads1299_port/ads1299_mcu_port.c`
- `ads1299_port/ads1299_mcuxpresso_adapter.c`
- `board/board_ads1299_binding.c`
- `board/board_ads1299_config_check.c` for compile-smoke validation

Use `cmake/official_sdk_parent_example.cmake` as the integration recipe. The parent project must provide the real MCUXpresso SDK target and the board macros above; the ADS1299 leaf does not replace vendor startup/linker/SDK files.

## Compile-smoke sequence

Enable the leaf's board binding, compile smoke, and example smoke in the parent project. Build:

1. `ads1299_rt1166_compile_smoke` — checks that the board contract, MCUXpresso adapter, portable port and shared core compile together.
2. `ads1299_rt1166_examples_smoke` — compiles the complete progressive example set.

A successful smoke build is only software evidence. Do not change this leaf to **BUILD-VERIFIED** until the documented reference configuration has actually compiled successfully; do not claim **BOARD-VERIFIED** until it has run on physical hardware.

## Bring-up order

Run the examples in this order:

1. `examples/probe_id.c` — reset, SDATAC, read ID, validate ADS1299 family.
2. `examples/internal_test.c` — configure the shared-core internal-test profile and acquire frames.
3. `examples/input_short.c` — validate the input-short path.
4. `examples/eeg_250sps.c` — configure normal inputs at 250 SPS and acquire frames.
5. `examples/stream_bounded.c` — use the bounded ring-buffer producer/consumer path; keep UART/USB/network output outside the DRDY-sensitive acquisition path.

`examples/main_ads1299.c` remains the combined reference flow. The progressive examples are preferred for first bring-up because each stage narrows the failure domain.

## What success means

For `probe_id.c`, success means the program reaches its success result after reading an ID accepted by the shared ADS1299 model/family validation. For the acquisition examples, success means configuration succeeds, DRDY becomes active, and complete ADS1299 frames are read without the example's staged error result changing to a failure code. Inspect the documented volatile result/frame counters in a debugger when no console transport is attached.

Do not interpret arbitrary electrode data as proof of correct hardware. First pass probe, internal test, and input-short checks.

## If something fails

- **Board/config compile failure:** one or more required `ADS1299_BOARD_*` definitions or SDK symbols are missing. Fix the parent board project, not `firmware/core_driver/ads1299/`.
- **Probe/ID failure:** re-check power, common ground, Mode 1, MSB-first, software CS, LPSPI clock source, wiring, RESET/PWDN state and conservative SPI rate.
- **DRDY timeout/no frames:** check START, DRDY direction/polarity/wiring and that configuration completed before RDATAC/START.
- **Streaming overflow:** the consumer/transport is slower than acquisition. Keep transport work out of the acquisition path and use the overflow counter to diagnose back-pressure.
- **Different board with MIMXRT1166:** keep the shared core, examples and port unchanged; regenerate/verify that board's pinmux/clock setup and change only the board-level `ADS1299_BOARD_*` definitions unless the board genuinely requires a different vendor-HAL integration.

## Authoritative platform references

Verify board revision, pinmux, clocks and SDK behavior against NXP's current **MIMXRT1160-EVK** product page, getting-started guide and `evkmimxrt1160` MCUXpresso SDK documentation. The repository deliberately keeps those vendor-owned details outside the portable ADS1299 core.
