# Rank 11 software reference gate — 2026-09-06

Status **Reference**. Software reference link and modeled fault tests passed;
rank 12 may proceed. Hardware boot/acquisition and CubeIDE/Keil execution have
not been tested. This is not qualification of all STM32G4 variants.

## Reproduce from this platform folder

```powershell
py -3 tests/compile_adapter.py `
  --device tests/.tools/cmsis-device-g4 --cube tests/.tools/cube-reference `
  --cmsis E:/PlatformIO/packages/framework-stm32cubeh7/Drivers/CMSIS/Core/Include `
  --cc E:/PlatformIO/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-gcc.exe
```

GCC 9.2.1, Cortex-M4, C11, -Wall -Wextra -Werror. Twelve real source files:
Core 4, standard Port 3, adapter, example, original board binding, genuine ST
system/startup. Official CubeG4 1.5.2 linker and genuine compiler CRT/newlib-nano
resources are used unchanged. Zero unresolved symbols. Final reference size:
text 13472, data 12, bss 1668 bytes (includes linker-reserved heap/stack).
Dependencies are pinned in sources.md. Omitting --cube checks objects only.
Build artifacts are temporary and removed automatically.

## Executed software tests

Run tests/run_polling_tests.py with the same --device/--cmsis/--cc arguments,
without --cube, using Python with Unicorn 2.1.4 and pyelftools 0.33. Locally the
existing RA6M5 tests/.tools Python environment was used. Tests compile the actual
adapter with real device definitions, then emulate ARM with modeled MMIO.

PASS: full-duplex and nullable buffers, byte-only DR writes, Mode 1/8-bit setup,
baud ceiling, missing RX/TXE, BSY completion, OVR/MODF/FRE faults, stale receive,
peripheral disabled after failure, invalid pins and refusing an enabled SPI.
An instruction limit also detects hangs. Board startup/DWT delay and physical
timing are not executed in this model.

Before Bench-tested: verify board routing and I/O levels, capture SCLK/CS/DRDY
and reset timing, read actual ID/registers, collect internal-test frames and log
loss/error counts. A 24-hour claim requires a separate continuous recorded run.
