# Build and validation boundary

Status: **Reference**. The FSP 5.6 reference ELF now links; no flash, ID read or bench test.

## Latest result: complete reference link, 2026-09-05

PASS: `tests/compile_bsp_headers.py` with the arguments below plus
`--link-reference` compiled **25 sources** and linked a complete reference ELF
against actual FSP startup, BSP, SPI/IOPORT drivers and the external linker script.
No fixture API or empty replacement driver is used in this build. The original
board binding is described in `board/reference_image.md`.

- GCC 9.2.1, FSP 5.6.0, TinyUSB 0.18.0 reference BSP configuration.
- GNU size: text 37760, data 232, bss 8952 bytes; undefined symbols: 0.
- ELF SHA256: `658f256ae5e2ed3aad5c0cf7842c0e09e585fa5b5b588f33b8bab2a9f70a3325`.
- Fresh temporary build, automatically removed; command rebuilds it locally.
- Frozen-timer fallback test added and passed in the ARM emulator.

This clears the **software reference-build gate**, permitting sequential work
on rank 9. It does not qualify board routing, runtime acquisition or FSP 6.5 BSP.
The catalog conservatively remains Reference, consistent with ranks 4–6.
Earlier checks and the failed 6.5 configuration combination are retained below.

## Executed on 2026-09-05

`tests/run_adapter_tests.py` compiles the actual adapter and test program into
ARM Thumb instructions with Arm GCC 9.2.1, then executes them in Unicorn 2.1.4.
It includes the unmodified SPI/IOPORT/transfer **API headers** from FSP 6.5.0;
`tests/fixtures/bsp_fixture.h` explicitly replaces the BSP for this isolated test.
It does NOT compile the real r_spi/r_ioport implementation or RA6M5 startup.

Result: PASS. Covered 65-byte chunking, real callback wait in the test model,
read-only/write-only transfers, GPIO levels, timer rollover, timeout, all defined
SPI error events, submit failure, close failure, duplicate pins, missing TEI,
wrong SPI mode, rejected DTC and sticky error precedence. Instruction-count cap
detects runaway execution. Mock tests cannot prove real IRQ or pin behavior.

```powershell
# Install requirements-test.txt into a local test-only virtual environment.
& tests/.tools/Scripts/python.exe tests/run_adapter_tests.py `
  --fsp E:/Temp/ads1299-fsp-6.5 `
  --cc E:/PlatformIO/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-gcc.exe
```

Run from this platform directory, or use absolute paths. `.tools` and `.uv-cache`
are ignored local tooling, not SDK distribution content. Test versions are pinned
in `tests/requirements-test.txt` (Python 3.12.8 used locally).

## Additional real-BSP compilation, 2026-09-05

PASS: **11 translation units** with `-Wall -Wextra -Werror`, Cortex-M33:
Core (4), Port (3), FSP adapter, example, official `r_spi.c` and `r_ioport.c`.
This second check does **not** include or bypass BSP using the test fixture.
It uses FSP **5.6.0**, pinned by TinyUSB **0.18.0**, with TinyUSB's external
RA6M5 BSP configuration and the installed CMSIS headers. A const-qualified
callback context compatibility issue was found and corrected. Emulator behavior
tests were then rerun successfully against both FSP 5.6.0 and 6.5.0 API headers.

```powershell
py -3 tests/compile_bsp_headers.py `
  --fsp tests/.tools/fsp-5.6 `
  --tinyusb tests/.tools/tinyusb-reference `
  --cmsis E:/PlatformIO/packages/framework-stm32cubeh7/Drivers/CMSIS/Core/Include `
  --cc E:/PlatformIO/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-gcc.exe
```

Original compile-only SPI settings are in `tests/fsp_config/r_spi_cfg.h`
(parameter checking on, DMA off). They are ordinary module configuration, not
mock APIs, but are not a complete board project. BSP/CMSIS, source and config
remain in ignored test dependencies, with upstream copyright files intact.

The same TinyUSB 0.18.0 config is **not** compatible with FSP 6.5.0 BSP as-is:
the real compilation stopped at missing `bsp_linker_info.h`. Do not mix these
versions or fabricate that generated header. FSP 6.5.0 remains API-tested only.
Neither compile-only check links a board image or verifies interrupt routing.

## Earlier missing-board-build finding (superseded for FSP 5.6 only)

The inspected official FSP 6.5 SPI example carries configurator input but not its
generated `ra_cfg/ra_gen` source output. No e2 studio/RASC installation was found
for this work. The older TinyUSB configuration enabled the additional 5.6
compile check above, but does not supply our SPI/timer/control-pin application.
Do not substitute fixture headers to declare `Compiles`. The new original
reference binding and complete 5.6 link above resolve this software gap without
claiming that 6.5 generated output has been obtained.

Follow `board/fsp_configuration.md` in an official generated project, selecting
verified GPIOs and a working independent timer. Add the sources directly, or use
this directory's CMake integration library with a real `ads1299_official_sdk`
usage target. Build Core once; exclude the old `port/board_sdk.h` declaration-only
path and exclude **all tests/fixtures** from production include paths.

For another generated board project, record exact versions, revision, build
command, link log, ELF hash and undefined symbols before asserting its build.
Next require logic-analyzer Mode 1/CS/reset timing, ID/register readback, internal
test frames and loss counters for hardware status. The 5.6 reference software
gate is met; hardware validation remains open.
