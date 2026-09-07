# Build status

Status is **Reference**. A real reference ELF link is now verified; the older
root validator's API-fixture compilation is separate, weaker evidence.

Use SysConfig in an official SDK 2.11.00.07 CCS project, add Core, generic Port
and this adapter, then record the real build output before status promotion.

## Real TI SDK build: 2026-09-05

SDK 2.11.00.07, Git commit `20807db79aa17b49f87ab8ec87f6b6d63ee2cb32`;
Arm GCC 9.2.1; CMake 3.16.4 and Ninja. Built all 10 repository C files plus
real external TI DriverLib sources and startup, then linked against TI's
MSPM0G3507 linker script. No vendor sources/startup/linker are redistributed.
CMSIS Core headers came from installed STM32CubeH7; hashes are emitted by
the verification script. All compiled C used -Wall -Wextra -Werror.

Result: ads1299_launchpad_reference.elf, zero undefined symbols.
Text/data/bss = 16240/276/76 bytes. No hardware execution or flashing.

```text
python tests/compile_official_sdk.py --sdk <TI-SDK-root> --cmsis <CMSIS-Core-Include> --cc <arm-none-eabi-gcc> --cmake <cmake> --ninja <ninja>
```

Temporary builds remain under this platform's tests directory and are cleaned
automatically. ELF size/hash and input header hashes are printed. To retain an
image, configure tests/cmake directly into a build directory under this platform.
Do not program NONMAIN/BCR/BSL configuration during ordinary bring-up. This
project does not provide or require custom security/debug-lock configuration.
