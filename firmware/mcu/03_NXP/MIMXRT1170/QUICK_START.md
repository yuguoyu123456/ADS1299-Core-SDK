# MIMXRT1170 ADS1299 Quick Start

Validation status: **TEMPLATE / NOT BUILD-VERIFIED / NOT BOARD-VERIFIED**.

## Reference board

Use the **NXP MIMXRT1170-EVKB** with its MIMXRT1176 device as the reference board for this leaf. Start from an official MCUXpresso SDK project for that board and the Cortex-M7 image. Keep vendor startup, linker, clock and pinmux ownership in that parent project.

## One board-level integration boundary

Do not edit `firmware/core_driver/ads1299/` for board bring-up. The board-specific LPSPI instance/clock and GPIO choices belong in the MCUXpresso parent project and are exposed through this leaf's `board/` and `mcuxpresso_adapter/` layers.

The supplied `board/evk_reference.c` is a reference binding, not evidence that a particular EVKB header wiring has been electrically verified. Confirm the installed board revision and generate/verify pinmux and clocks with MCUXpresso Config Tools before connecting hardware.

## Wiring contract

Connect the selected LPSPI signals to ADS1299 `SCLK`, `DIN`, and `DOUT`. Use software-controlled GPIO for `CS`, `RESET`, `PWDN`, and `START`; route `DRDY` to an input GPIO. Share digital ground and use a separately verified ADS1299 power/reference/electrode front end.

Configure SPI as **Mode 1 (CPOL=0, CPHA=1), MSB first**. Begin at a conservative SPI clock (1 MHz is recommended for initial bring-up) and increase it only after probe and self-test are reliable.

## Files used by the platform integration

The RT1170 CMake leaf combines the shared ADS1299 core with:

- `ads1299_port/` — portable RT1170-facing port glue;
- `mcuxpresso_adapter/` — NXP LPSPI/GPIO adapter;
- `board/evk_reference.c` — opt-in reference board binding;
- `board/board_ads1299_binding_check.c` — compile-time board binding contract;
- `examples/` — progressive bring-up programs.

Use `cmake/official_sdk_parent_example.cmake` as the parent-project integration recipe. The parent project must supply the real MCUXpresso SDK target, include paths, board libraries, CPU/compiler options, and the board configuration generated for the actual EVKB revision. This repository intentionally does not invent vendor startup or linker files.

## Compile-smoke sequence

From the consuming official SDK project, enable the RT1170 board binding and build:

1. `ads1299_rt1170_compile_smoke` — checks board binding, MCUXpresso adapter, platform port and shared core together.
2. `ads1299_rt1170_examples_smoke` — compiles the progressive examples against the same API contract.

A successful smoke compile is software evidence only. Keep this leaf at TEMPLATE until that documented configuration has actually compiled; BOARD-VERIFIED additionally requires execution on physical hardware.

## Bring-up order

Run the examples in this order:

1. `examples/probe_id.c` — reset, SDATAC, read ID and validate the ADS1299 family.
2. `examples/internal_test.c` — use the shared-core internal-test configuration and acquire frames.
3. `examples/input_short.c` — validate the input-short path before attaching electrodes.
4. `examples/eeg_250sps.c` — configure normal inputs at 250 SPS and acquire EEG frames.
5. `examples/stream_bounded.c` — use the fixed bounded producer/consumer queue so transport work does not block the DRDY-sensitive path.

`examples/main_ads1299.c` remains the combined reference flow. For first bring-up, prefer the progressive examples because each stage narrows the failure domain.

## What success means

`probe_id.c` succeeds only after the shared core accepts the returned device identity as an ADS1299-family device. Acquisition examples must complete configuration, observe DRDY and read complete frames without their staged result variable entering a failure state. When no console transport is attached, inspect the documented volatile result, frame and overflow variables in the debugger.

Do not use arbitrary electrode-looking waveforms as the first proof of correct hardware. Pass ID/probe, internal-test and input-short validation first.

## Failure isolation

- **Board/config compile failure:** fix missing SDK/board symbols, pinmux, clocks or board binding in the parent project; do not edit the ADS1299 core.
- **SPI/ID failure:** check power, common ground, Mode 1, MSB-first, software CS, RESET/PWDN state, wiring and conservative SPI clock.
- **DRDY timeout/no frame:** check START and DRDY wiring/direction and ensure configuration completed before continuous acquisition.
- **Streaming overflow:** the consumer is slower than acquisition; keep UART/USB/network work outside the DRDY-sensitive producer and use the overflow counter to diagnose back-pressure.
- **Another board using MIMXRT1170:** preserve the shared core and examples; replace only the board-specific pinmux/clock/GPIO/LPSPI binding unless the board genuinely requires a different vendor-HAL adapter.

## Validation boundary

Current status is **TEMPLATE / NOT BUILD-VERIFIED / NOT BOARD-VERIFIED**. Do not promote it based on source review or host compilation alone. Record BUILD-VERIFIED only after the documented MCUXpresso reference configuration compiles successfully, and BOARD-VERIFIED only after the corresponding physical-board bring-up has been run and evidenced.
