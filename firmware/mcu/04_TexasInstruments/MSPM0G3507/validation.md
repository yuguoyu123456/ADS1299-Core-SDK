# Validation

Current status: **Reference**

Allowed lifecycle states: `Planned`, `Reference`, `Example`, `Compatible`,
`Compiles`, `Bench-tested`, `24h-tested`.

A real TI SDK 2.11.00.07 reference ELF linked on 2026-09-05; see build.md for exact scope. Hardware execution is unverified.

To advance status, attach exact SDK/compiler versions, the clean command and
log. `Bench-tested` additionally requires a real ADS1299 ID read, internal-test
waveform and packet integrity evidence. `24h-tested` requires loss/CRC/error
counts from a continuous 24-hour run.
