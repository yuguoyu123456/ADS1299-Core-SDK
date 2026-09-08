# ADS1299 core driver: host build and smoke test

This directory is portable C. The existing ESP-IDF component integration remains supported, and the same `CMakeLists.txt` now also supports a normal desktop CMake build for compile checks and offline regression tests.

## Prerequisites

Use CMake 3.16 or newer and a C99 compiler (GCC or Clang are recommended). No ADS1299 board is required for this software-only smoke test.

## Linux / macOS / WSL

From this directory:

```sh
cmake -S . -B build -DADS1299_BUILD_HOST_TESTS=ON
cmake --build build --clean-first
ctest --test-dir build --output-on-failure
```

Expected result:

```text
100% tests passed, 0 tests failed
```

Running the executable directly should finish with:

```text
ADS1299 core host self-test: PASS
```

## Windows

With Visual Studio Build Tools installed, open a Developer PowerShell in this directory:

```powershell
cmake -S . -B build -DADS1299_BUILD_HOST_TESTS=ON
cmake --build build --config Release --clean-first
ctest --test-dir build -C Release --output-on-failure
```

With Ninja and GCC/Clang, the Linux-style commands can be used unchanged.

## What the test covers

`ads1299_core_selftest.c` exercises code that does not need physical GPIO/SPI hardware:

- ADS1299-4/-6/-8 frame lengths: 15 / 21 / 27 bytes.
- ADS1299 status-header validation (`1100b`).
- Eight-channel 24-bit signed sample decoding, including both full-scale endpoints.
- Invalid frame length and invalid status-header rejection.
- 24-bit two's-complement sign extension.
- Register safety model behavior, including read-only rejection and reserved data-rate rejection.
- CONFIG1 sanitization while preserving the 250-SPS request.
- Nominal 250-SPS interpretation at a 2.048-MHz clock.
- Gain-code semantic interpretation.

## What this test does not prove

A PASS is only a portable software regression result. It does **not** prove real SPI timing, DRDY interrupt timing, RESET/START wiring, clock integrity, analog power/reference behavior, signal quality, daisy-chain timing, DMA/cache correctness on a particular MCU, EMC performance, electrical safety, medical-device compliance, or production readiness. Those items require the target platform and real hardware validation.

## ESP-IDF compatibility

When this directory is consumed by ESP-IDF, CMake detects `idf_component_register` and keeps the original component registration path. The standalone host branch is used only when this directory is configured as a normal CMake project.

## Beginner rule

Run this host test before debugging a new board port. If it passes but the physical board does not start, focus first on the platform `board/config/port` layer: SPI Mode 1 (CPOL=0, CPHA=1), MSB-first, CS, DRDY, RESET, START, clock/reference/power, and the exact MCU peripheral instance. Do not edit the portable core solely to compensate for a board-wiring or HAL configuration error.
