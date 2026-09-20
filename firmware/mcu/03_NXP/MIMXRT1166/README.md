# MIMXRT1166 ADS1299 Port

Global ecosystem rank: **136**. Validation status: **TEMPLATE / NOT BUILD-VERIFIED / NOT BOARD-VERIFIED**. Hardware validation is not implied.

> New users: start with [`QUICK_START.md`](QUICK_START.md). It is the maintained bring-up path for this model folder.

## Platform

- Vendor: NXP
- MCU: MIMXRT1166
- Reference board: **NXP MIMXRT1160-EVK** (MIMXRT1166 device)
- Official environment: MCUXpresso SDK
- Build integration: `CMakeLists.txt` plus `cmake/official_sdk_parent_example.cmake`
- Board-dependent configuration: `board/board_ads1299_config.h`

The repository does not invent a fixed EVK header pin assignment. Select a suitable LPSPI instance and GPIOs in the official MCUXpresso board project, then expose them through the single `ADS1299_BOARD_*` board contract documented by the Quick Start and `board/` files.

## ADS1299 connection

Use SPI Mode 1 (CPOL=0, CPHA=1), MSB first. Keep CS software-controlled and route DRDY, RESET, PWDN and START as independent GPIOs. Start at 4 MHz or less until ID read, configuration readback and the internal test signal pass. Confirm every physical header pin against the exact EVK revision before wiring.

## Repository layers

- Shared ADS1299 behavior: `../../../core_driver/ads1299/`
- Portable MIMXRT1166 port: `ads1299_port/`
- MCUXpresso SDK adapter: `mcuxpresso_adapter/`
- One board/config layer: `board/`
- Official-SDK parent-project recipe: `cmake/official_sdk_parent_example.cmake`
- Progressive examples: `examples/`
- Detailed integration notes: `integration.md`

The platform layer does not define ADS1299 registers. Hardware-dependent LPSPI, GPIO, clock and pin choices belong in the board/SDK integration layer. UART, USB or Ethernet transport must remain outside the DRDY timing path; `examples/stream_bounded.c` demonstrates bounded producer/consumer buffering.

## Progressive bring-up contract

Use the examples in this order:

1. `examples/probe_id.c` — reset, SDATAC and ADS1299-family ID validation.
2. `examples/internal_test.c` — shared-core internal test profile and frame acquisition.
3. `examples/input_short.c` — shared-core input-short validation path.
4. `examples/eeg_250sps.c` — normal-input, 250-SPS EEG acquisition.
5. `examples/stream_bounded.c` — fixed-capacity streaming buffer with overflow diagnostics.
6. `examples/main_ads1299.c` — combined integration example after the staged tests pass.

With `ADS1299_RT1166_EXAMPLE_SMOKE=ON`, the CMake integration compiles the progressive examples as separate object targets so their independent `main()` functions do not collide. `ADS1299_RT1166_COMPILE_SMOKE=ON` also compiles the board configuration contract. These are compile contracts, not evidence of successful hardware operation.

## What a new board should change

For another board using the same MCU, keep the shared ADS1299 core and the MIMXRT1166 port unchanged. Change only the parent MCUXpresso project's actual pinmux/clock setup and the values supplied through the board/config contract. If an initial bring-up appears to require editing `ads1299.c`, `ads1299_regs.h` or `ads1299_model.c`, treat that as an integration defect rather than the normal user path.

## Validation boundary

The model folder now contains a real MCUXpresso adapter, board contract, progressive examples, compile-smoke targets and an official-SDK parent-project integration recipe. It is therefore no longer an empty/planned callback placeholder.

It remains **TEMPLATE / NOT BUILD-VERIFIED / NOT BOARD-VERIFIED** until the documented targets have actually compiled in a real MIMXRT1160-EVK MCUXpresso SDK project. Do not interpret repository structure, host inspection, or CMake target presence as board validation, timing validation, electrical-safety validation or production readiness.

[Official NXP entry](https://www.nxp.com/) · [101–200 ecosystem list](../../ECOSYSTEM_101_200.md)
