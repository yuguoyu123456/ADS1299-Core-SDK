# Rank 10 build progress — 2026-09-06

Status **Reference**. Software reference gate passed; rank 11 may proceed.
Physical cold boot, wiring and acquisition remain unverified.

PASS: 28 actual translation units compiled and linked with GCC 9.2.1, Cortex-M4,
strict warnings, real CY8C624ABZI_S2D44/PDL/BSP/CMSIS headers and no fixtures.
Uses genuine GCC CRT objects, newlib-nano/nosys and unmodified official startup
and linker resources. CM0+ is the official prebuilt CM0P_SLEEP image, NOT locally
rebuilt from source. There are zero unresolved symbols.

```powershell
py -3 tests/compile_adapter.py `
  --pdl tests/.tools/pdl --core-lib tests/.tools/core-lib `
  --bsp tests/.tools/bsp --cm0p tests/.tools/cm0p `
  --cmsis E:/PlatformIO/packages/framework-stm32cubeh7/Drivers/CMSIS/Core/Include `
  --cc E:/PlatformIO/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-gcc.exe
```

Run from this platform folder. Temporary objects are removed automatically.
Dependency revisions are in sources.md. This is a GNU reference link, not a
ModusToolbox IDE build, programming-package validation or executed boot test.
Without --cm0p the script performs object-only checks.

## Checked link layout

CM0P image: 6216 bytes at 0x10000000; CM4 vectors at 0x10002000. CM4 .text:
23280 bytes; .bss: 572 bytes. Official linker also reserves a large heap and
4096-byte stack. Berkeley size counts the heap in its aggregate text column;
do not report that aggregate as flash code size. Use size -A.

Observed ELF SHA256:
`11566980a73e409f6d59680415ef267f120ee28d6e3855ee72c6d8bce836e8ed`.
This identifies that run only; temporary paths/toolchain metadata may affect hashes.

## Executed software behavior tests

tests/run_polling_tests.py accepts the same --pdl/--core-lib/--bsp/--cmsis/--cc
arguments, without --cm0p. Python requires Unicorn 2.1.4 and pyelftools 0.33.
Executed with the existing RA6M5 tests/.tools Python environment. Actual adapter,
cy_scb_spi.c and cy_scb_common.c execute as ARM instructions against modeled MMIO.
PASS: full duplex, zero-filled read, discarded read, invalid length, missing RX,
busy shifter, overflow, stale RX, full TX, empty read, sticky fault/disabled SCB,
negative GPIO read, and invalid mode/pin. This is not a physical timing test.

Hardware ID/readback/test-waveform/timing/loss-counter validation remains separate.
Clock and provisional GPIO assignments are documented in board/reference_image.md.

The library CMakeLists.txt intentionally requires a real `ads1299_official_sdk`
usage target. It does not import vendor source, generate a dummy BSP, or copy
startup/linker files into shipping source. board/reference_image.c supplies the
cold-start reference board_ads1299_hal(); do not link a second board binding.
