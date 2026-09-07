# Software validation — 2026-09-06

Catalog status remains **Reference**. Software reference-build gate passed;
hardware operation, exact board routing and continuous acquisition unverified.

## Complete reference link

`tests/compile_adapter.py` compiled 14 C translation units with real device/CMSIS
headers and linked a complete ELF with zero undefined symbols. Compiler: Arm
GCC 9.2.1; no fixture APIs. Core is linked once and is unchanged.

Final debugger-snapshot example build: text 30584, data 120, bss 65632 bytes.
ELF SHA256: `de475b89c72367922c818a6a2244e4e0e7660769323326f5552d5ffb9610f7da`.

```powershell
py -3 tests/compile_adapter.py `
  --reference tests/.tools/csp-reference `
  --cc E:/PlatformIO/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-gcc.exe `
  --gcc-reference tests/.tools/gcc-reference `
  --linker tests/.tools/same54p20a_flash.ld
```

Run from this platform directory. The clean temporary build is removed after
checks. Omit the final two arguments for compile-only verification. The external
linker reserves 64 KiB stack, explaining the large reported BSS. See sources.md
for exact dependency revisions, provenance and licenses. This is **not** a
verified MPLAB X/XC32 build: the vendor XC32 startup requires its own runtime.

## Behavior tests

`tests/run_polling_tests.py` compiles only the original adapter/test harness and
executes them with Unicorn modeled peripheral registers. It uses official DFP
types and offsets, not real peripherals. Requires Python, unicorn 2.1.4 and
pyelftools 0.33 (already available in rank 8's local test environment).

```powershell
python tests/run_polling_tests.py --reference tests/.tools/csp-reference `
  --cc E:/PlatformIO/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-gcc.exe
```

PASS: normal/null transfers, invalid inputs, Mode 1 and baud ceiling, stale RX,
overflow, missing RX, missing final TXC and synchronization stall. An instruction
cap detects runaway test execution. These checks do not establish physical SPI
edge timing, I/O voltage safety, ADC ID or signal quality.

## User project

Add Core (4 C), standard Port (3 C), harmony_adapter C and your board binding.
Use CMakeLists.txt only from a parent supplying `ads1299_official_sdk`, or add
sources in MPLAB. Do not also compile Harmony SPI PLIB/ISR/DMA for this SERCOM.
Use board/reference_image.c only with its documented cold-start clock profile.
The minimal example uses internal test input; no electrodes or host transport
are necessary for the initial bring-up. Hardware validation remains a separate
ID/readback/waveform/timing/loss-counter gate before any Bench-tested claim.
