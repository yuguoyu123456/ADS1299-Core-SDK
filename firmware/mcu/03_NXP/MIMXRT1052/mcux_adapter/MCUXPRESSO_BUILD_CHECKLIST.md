# MIMXRT1052 MCUXpresso build-integration checklist

Status: **TEMPLATE / not yet BUILD-VERIFIED / not BOARD-VERIFIED**.

This checklist is the hand-off between the repository port and an official NXP MCUXpresso SDK project. It deliberately does not copy vendor startup files, linker scripts, generated pin mux code, or SDK sources into this repository.

## 1. Start from the vendor project

Create/open an official MCUXpresso SDK project whose selected device is **MIMXRT1052** and whose board/device configuration matches the hardware actually on the bench. Keep the SDK-generated clock, pin-mux, startup and linker files owned by that project.

Do not claim this repository is build-verified until the checks below have actually compiled in that project.

## 2. Add repository include paths

Add include paths for:

- `firmware/core_driver/ads1299`
- `firmware/mcu/03_NXP/MIMXRT1052/ads1299_port`
- `firmware/mcu/03_NXP/MIMXRT1052/mcux_adapter`
- `firmware/mcu/03_NXP/MIMXRT1052/board`

Use the existing shared ADS1299 core. Do not copy register definitions into the MCUXpresso project.

## 3. Add sources without reducing the shared-core union

Add the ADS1299 core sources required by the repository's existing build/API surface, plus:

- the MIMXRT1052 `ads1299_port/*.c` sources;
- the existing `mcux_adapter` implementation;
- `board/board_ads1299_context_example.c` (or an application-owned equivalent using the same config contract).

For compile-contract validation also add, one at a time if the project only permits one `main`/test target:

- `board/board_ads1299_config_check.c`;
- `mcux_adapter/compile_smoke.c`.

Do not fix a compile error by removing an established shared-core module. Resolve the include/API/configuration error instead.

## 4. Fill exactly one board configuration point

Edit `board/board_ads1299_config.h` (or provide the documented application definitions before including it) with the values from the **actual MCUXpresso-generated board project**:

- LPSPI peripheral instance;
- LPSPI source clock in Hz;
- ADS1299 SPI baud rate;
- software-CS GPIO and pin;
- RESET GPIO and pin;
- PWDN GPIO and pin;
- START GPIO and pin;
- DRDY GPIO and pin;
- microsecond delay callback.

Do not guess connector pins. Confirm the exact board revision/schematic and the generated pin mux. ADS1299 SPI remains Mode 1 (CPOL=0, CPHA=1), MSB first, with software-controlled CS.

## 5. Compile in increasing scope

Use the MCUXpresso project's normal **Build** command and record the SDK version, IDE/toolchain version, selected device/board and build configuration.

Compile in this order so failures have a narrow meaning:

1. `board_ads1299_config_check.c` — board contract/macros and context types.
2. `mcux_adapter/compile_smoke.c` — MCUX SDK adapter ↔ portable port interface.
3. `examples/probe_id.c` — shared core + board HAL + basic command/register path.
4. `examples/internal_test.c`.
5. `examples/input_short.c`.
6. `examples/eeg_250sps.c`.
7. `examples/stream_bounded.c`.

A successful compile is **BUILD-VERIFIED only for the exact recorded configuration**. It does not imply that SPI wiring, DRDY timing, electrodes, or the ADS1299 hardware were tested.

## 6. Flash/run only after a clean build

For hardware bring-up, run the examples in the same order. Probe/ID must pass before the signal examples. Internal-test and input-short should be used before connecting electrodes. Keep transport/printing out of the DRDY timing-critical path; the bounded-stream example is the reference pattern for decoupling acquisition from a slower consumer.

## 7. Evidence to record before changing validation status

For **BUILD-VERIFIED**, record at minimum:

- exact MIMXRT1052 board/device selection;
- MCUXpresso SDK version;
- compiler/IDE version;
- build configuration;
- which smoke/example targets compiled;
- commit SHA.

For **BOARD-VERIFIED**, additionally record physical board revision, ADS1299 hardware used, wiring/config revision, observed probe ID and which self-test/acquisition stages actually ran. Do not infer board verification from a successful build.

## Failure classification

- `board_ads1299_config.h` error: board/config contract incomplete.
- missing/unknown MCUX SDK symbol: wrong SDK component, include, selected device, or adapter/API drift.
- probe/ID failure after successful build: investigate power, SPI Mode 1, software CS, clock, RESET/PWDN and wiring before changing shared core.
- DRDY timeout: investigate START/DRDY wiring and conversion state.
- streaming overflow with acquisition otherwise valid: consumer/transport is too slow; preserve bounded buffering and overflow accounting rather than blocking acquisition.
