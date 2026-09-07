# Build status

Status is **Reference**, with a real RAM-reference image build recorded below.
The older root acceptance script checks only an API fixture; it is not the
source of this new evidence.

Create an official `evkbmimxrt1060` project, add Core, generic Port and adapter,
generate the pin mux, then record exact SDK/toolchain output before promotion.

## Real SDK build, 2026-09-05

NXP SDK `MCUX_2.16.000`, commit `6f3fd257cdcf978a4d26e7d6e9eed9240037422b`,
Arm GCC 9.2.1, CMake 3.16.4 and Ninja compiled 11 repository C files and linked
`ads1299_evkb_reference.elf`. External SDK sources include the actual EVKB
clock configuration, LPSPI/GPIO/clock drivers, startup and RAM linker script.
None are copied into this repository. External CMSIS Core headers came from
the installed STM32CubeH7 package; their hashes are printed by the check.
SDK 25.06.00 remains unverified, not the version used for this result.

ELF undefined symbols: 0. Text/data/bss: 47108/280/2192 bytes.
The image was not flashed or run. It is a RAM-debug reference, not a Flash/XIP
boot image. Use the official SDK debugger initialization and verify the
ITCM/DTCM partition before loading. No acquisition or long-duration claim.

```text
python tests/compile_official_sdk.py --sdk <SDK-root> --cmsis <CMSIS-Core-Include> --cc <arm-none-eabi-gcc> --cmake <cmake> --ninja <ninja> --link-reference-ram
```

Repository C uses `-Wall -Wextra -Werror`. SDK unused-parameter warnings stay
visible but are not promoted to errors; SDK headers use system include paths.
Temporary build products stay under this target's tests directory and are
removed automatically. The script checks undefined symbols and prints ELF
size/hash. For an image to keep, configure tests/cmake directly into a build
directory under this platform, using the same command-line definitions.
