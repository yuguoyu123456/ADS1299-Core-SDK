# Validation

Current status: **Compiles**

Allowed lifecycle states: `Planned`, `Reference`, `Example`, `Compatible`,
`Compiles`, `Bench-tested`, `24h-tested`.

Build entrypoint: `platformio run -d firmware/mcu/01_STMicroelectronics/STM32F407`. Clean local build passed on 2026-09-04.

To advance status, attach exact SDK/compiler versions, the clean command and
log. `Bench-tested` additionally requires a real ADS1299 ID read, internal-test
waveform and packet integrity evidence. `24h-tested` requires loss/CRC/error
counts from a continuous 24-hour run.


The clean build used STM32CubeF4 1.28.3; see `build.md` for exact tool versions,
vendor warnings and memory output. This proves compilation/linking only.
