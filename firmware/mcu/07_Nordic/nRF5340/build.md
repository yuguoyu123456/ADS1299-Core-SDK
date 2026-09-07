# Local hardware-SPI build evidence

Built on 2026-09-05 for `nrf5340dk/nrf5340/cpuapp`, secure application core,
without sysbuild/TF-M or a network-core image:

- Zephyr v3.7.1: `9f824289b28d7aea2eee74f62787c385a5005453`.
- CMSIS: `4b96cbb174678dcd3ca86e11e1f24bc5f8726da0`.
- hal_nordic: `ab5cb2e2faeb1edfad7a25286dcb513929ae55da`.
- Arm GCC 9.2.1, cross-compile variant; Python 3.12.8, CMake 3.29.6,
  Ninja 1.11.1, Python dependencies in tests/requirements-build.txt.

The linked zephyr.elf uses CONFIG_SPI=y and CONFIG_NRFX_SPIM4=y. Undefined
symbols: 0. The initial build reports FLASH 28804 B, RAM 5576 B; GNU size
reports text/data/bss = 27976/816/5309 bytes (different accounting).
No hardware was flashed or executed. The existing NCS 3.4.0 CI definition
has not been run here and is not evidence for that SDK version.

## Reproduce a fresh build

Install the Python requirements into a local test virtual environment. Supply
the pinned official sources externally; the CMSIS module folder must be
named `cmsis` because Zephyr 3.7's module descriptor derives its name from
the folder. No vendor SDK, startup or linker is copied into this project.

```text
python tests/compile_zephyr.py --zephyr <zephyr-v3.7.1> --cmsis <modules/cmsis> --nordic <hal_nordic> --cc <arm-none-eabi-gcc> --cmake <cmake> --ninja <ninja>
```

Run that command using the Python environment containing the requirements.
The script creates a fresh temporary build/cache inside this platform's tests
directory, checks SPIM4 selection and undefined symbols, prints ELF size/hash,
then removes temporary products. tests/.tools and tests/.build are ignored
local tooling/build directories. Do not commit or distribute them as SDK code.

Native dtc was unavailable; Zephyr's Python devicetree generation succeeded,
but optional dtc diagnostics did not run. Compiler success is not runtime,
electrical, register-readback or sustained-sampling validation.
