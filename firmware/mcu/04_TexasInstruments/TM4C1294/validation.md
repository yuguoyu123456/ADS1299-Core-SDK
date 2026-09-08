# Validation

Current status: **Reference**

Allowed lifecycle states: `Planned`, `Reference`, `Example`, `Compatible`,
`Compiles`, `Bench-tested`, `24h-tested`.

A complete GNU reference ELF linked on 2026-09-07 using external Energia startup/linker and its TI DriverLib 2.1.4.178 sources. Vectors, stack and zero unresolved symbols checked; five real-driver/model fault scenarios passed. See build.md. No TI SDK 2.2, CCS build or board execution claim.

To advance status, attach exact SDK/compiler versions, the clean command and
log. `Bench-tested` additionally requires a real ADS1299 ID read, internal-test
waveform and packet integrity evidence. `24h-tested` requires loss/CRC/error
counts from a continuous 24-hour run.
