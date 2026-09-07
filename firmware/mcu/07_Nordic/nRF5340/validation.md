# Validation

Current status: **Compiles**

Allowed lifecycle states: `Planned`, `Reference`, `Example`, `Compatible`,
`Compiles`, `Bench-tested`, `24h-tested`.

Build entrypoint: `tests/compile_zephyr.py` (see build.md). Clean local Zephyr 3.7.1 hardware-SPIM4 ELF build passed on 2026-09-05. NCS 3.4.0 CI was not run; no hardware test.

To advance status, attach exact SDK/compiler versions, the clean command and
log. `Bench-tested` additionally requires a real ADS1299 ID read, internal-test
waveform and packet integrity evidence. `24h-tested` requires loss/CRC/error
counts from a continuous 24-hour run.
