# Build status

## EVK-derived RAM reference image (2026-09-05)

Following the user's similar-board authorization, `board/evk_reference.c`
provides an explicit EVK-derived profile. 11 repository C files compile and
`ads1299_evk_reference.elf` links with NXP SDK 2.16.000's real runtime,
startup and RAM linker script, all retained outside this repository. The ELF
has zero undefined symbols and size text/data/bss = 44672/280/2192 bytes.
It was not loaded onto hardware. This is a RAM-debug image, not an XIP/flash
boot image, and must not be flashed as one. Check ITCM/DTCM partition and the
official SDK debug-launch configuration before loading it into RAM.

Append `--cmake <cmake> --ninja <ninja> --link-reference-ram` to the command
below. The check prints ELF size/hash and fails on any undefined symbol.
Vendor unused-parameter warnings remain visible but are not errors; repository
sources retain `-Wall -Wextra -Werror`. SDK headers are system includes.
Catalog status remains Reference because it describes EVKB, not this EVK
reference profile. See board/evk_reference.md for wiring and limitations.

The older object-only milestones below are retained as a progress record.

## Official-header compilation evidence (2026-09-05)

Executed `tests/compile_official_sdk.py` with NXP SDK tag `MCUX_2.16.000`,
commit `6f3fd257cdcf978a4d26e7d6e9eed9240037422b`, external CMSIS Core
headers from the installed STM32CubeH7 package, and Arm GCC 9.2.1.
All 10 translation units (4 Core, 3 Port, 2 adapter, minimal example) compiled
to Cortex-M7 objects with `-std=c11 -Wall -Wextra -Werror`.
The script prints header SHA-256 values to identify exact input versions.
SDK 25.06.00 remains a proposed integration version; it was not compiled.

Reproduce from this directory:

```text
python tests/compile_official_sdk.py --sdk <MCUX_2.16.000-root> --cmsis <CMSIS-Core-Include> --cc <arm-none-eabi-gcc>
```

Outstanding: board initialization implementation, verified control-pin wiring,
full firmware link, and hardware acquisition. Reference status is retained.

`ads1299_mcuxpresso_init()` now supplies peripheral initialization; the board
still owns muxes, clock roots and the actual GPIO assignments. `CMakeLists.txt`
exports a static library for a consuming official SDK application, not a
standalone firmware image. CMake 3.16.4 + Ninja configuration and the build of
`libads1299_rt1170.a` plus the example object passed on 2026-09-05 with the
same SDK/compiler. This is an archive build, not a resolved executable link.

To reproduce that additional check, append
`--cmake <cmake-executable> --ninja <ninja-executable>` to the command above.
Temporary objects and build files are generated under this platform's `tests/`
and automatically removed after the check. No SDK implementation is copied
into this repository.

The maintained status is **Reference**.

`tests/validate_mimxrt1170.py` syntax-compiles the adapter against a minimal
fixture containing only the MCUXpresso API surface it uses. This proves the
repository-owned C boundary is self-consistent, not a full EVKB firmware build.

For a real image, create an `evkbmimxrt1170` Cortex-M7 project in the official
SDK, add Core, the generic Port and this adapter, generate the pin mux, and use
the SDK toolchain. Record exact versions and output before claiming Compiles.
