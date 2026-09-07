# Rank 12 software reference evidence — 2026-09-06

Status **Reference**. Software reference gate passed; rank 13 may proceed.
Hardware boot/acquisition and TrustZoneEnabled are unverified.

PASS: twelve real source files compiled and linked with genuine STM32U575xx/CMSIS
headers, GCC 9.2.1, Cortex-M33, FPv5-SP hard-float, C11, strict warnings. Includes
Core 4, Port 3, adapter/example, original board binding, official system/startup.
Official external CubeU5 1.1.0 linker and genuine compiler CRT are unchanged.
Zero unresolved symbols; vectors at 0x08000000. Final size: text 15208, data 24,
bss 1672 bytes (including linker heap/stack reservations). No physical boot claimed.

Run from this directory:

```powershell
py -3 tests/compile_adapter.py `
  --device tests/.tools/cmsis-device-u5 --cube tests/.tools/cube-reference `
  --cmsis E:/PlatformIO/packages/framework-stm32cubeh7/Drivers/CMSIS/Core/Include `
  --cc E:/PlatformIO/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-gcc.exe
```

The adapter is specifically SPI1 on U575, not a blanket claim for all U5 parts.
It owns packet size/start/end handling, byte FIFO access, nullable buffers,
bounded waits, fault latching and GPIO callbacks. ADS1299 behavior remains in
Core. No IRQ/DMA/HAL or autonomous-trigger sharing is supported.

## Executed model tests

tests/run_polling_tests.py takes --device/--cmsis/--cc, without --cube. Executed
using the existing RA6M5 tests/.tools Python with Unicorn 2.1.4/pyelftools 0.33.
Actual adapter ARM instructions run against modeled MMIO. PASS: nullable buffers,
byte accesses, missing RX/TX/EOT, faults/stale RX, sticky failure, EOT delay and
delay failure, 65535+1 splitting with CS continuity, actual-baud guard calculation.
This does not emulate TrustZone, board startup, DWT timing or electrical behavior.

ES0499 guard rationale and provisional pin/clock profile: board/reference_image.md.
Omitting --cube checks objects only. This is not a CubeIDE or secure-loader build.

Physical boot, ADS1299 ID, register readback, test waveform and sustained capture
remain unverified. External dependencies are ignored in tests/.tools. No full
vendor SDK is included in shipping source.
