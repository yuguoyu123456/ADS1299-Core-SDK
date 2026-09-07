# Reproducible software checks

Current status: **Reference**

2026-09-07: GNU Arm Embedded 9.2.1, MCUX_2.16.000 commit
6f3fd257cdcf978a4d26e7d6e9eed9240037422b; CMSIS Core headers supplied externally
from the installed STM32CubeH7 package. CPU is Cortex-M33 Core0, soft-float.

From repository root (replace dependency paths with your installations):

```powershell
py -3 firmware/mcu/03_NXP/LPC55S69/tests/compile_adapter.py --link --sdk firmware/mcu/03_NXP/LPC55S69/tests/.tools/mcux-sdk --cmsis E:/PlatformIO/packages/framework-stm32cubeh7/Drivers/CMSIS/Core/Include --cc E:/PlatformIO/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-gcc.exe
```

The script compiles shared Core, Port, original adapter/example/board binding,
real vendor SPI/Flexcomm/GPIO/clock/reset/system/startup and links the unchanged
Core0 flash linker with genuine GNU CRT/newlib-nano. The temporary ELF is checked
then removed by the script. No flash command is issued. Supply SDK sparse paths
drivers/common, drivers/flexcomm, drivers/lpc_gpio, drivers/lpc_iocon,
devices/LPC55S69 and boards/lpcxpresso55s69; keep dependencies in tests/.tools.

Initial full link passed: 17 objects, vectors at 0, no undefined symbols.
The script also checks that board, real SPI and Core frame-read symbols survived
linker garbage collection, so an interlock cannot hide missing integration code.
All project code uses Wall/Wextra/Werror. SDK SPI, GPIO and clock sources retain
visible unused-parameter warnings with that category made nonfatal only for
those vendor compilation units. This is not a warning-free build claim.

Behavior tests need Python with Unicorn 2.1.4 and pyelftools 0.33. Both scripts
accept the same `--sdk`, `--cmsis`, `--cc` arguments:

- `tests/run_adapter_tests.py`: genuine SDK types, SPI API doubles. Passed
  boundary/chunking, null buffers, mode/divider, CS ownership, stale RX, FIFO
  errors, API failure, non-idle timeout, latched faults and safe CS release.
- `tests/run_vendor_tests.py`: executes original fsl_spi.c and the adapter on
  an ARM instruction emulator with a narrow modeled FIFO. Passed 65-byte transfer
  with three EOT chunks and continuous physical CS, TX stall timeout, post-FIFO
  non-idle timeout and RX-error rejection. Flexcomm initialization is doubled;
  the model is not cycle accurate and does not validate clock/reset or wiring.

The test SPI retry budget is 1000, production reference 100000; both are finite
iteration counts, not measured timeouts. API-double tests do not prove SDK driver
behavior; real-driver tests do not prove peripheral hardware behavior.

Software reference gate: met with documented provisional wiring and boot interlock.
Remaining hardware gates: exact control-pin review, physical boot, logic-analyzer
mode/CS/DRDY checks, real ADS1299 ID/test waveform, packet/loss accounting and 24h.
No MCUXpresso IDE build, flashing, Bench-tested or 24h-tested claim is made.
