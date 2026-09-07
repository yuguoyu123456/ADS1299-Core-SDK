# Validation

Current status: **Reference**

Allowed lifecycle states: `Planned`, `Reference`, `Example`, `Compatible`,
`Compiles`, `Bench-tested`, `24h-tested`.

A GNU CM4 reference ELF including the official prebuilt CM0P_SLEEP image linked on 2026-09-06; image addresses and zero unresolved symbols checked. Real PDL/adapter modeled-MMIO tests passed. See build.md. No physical boot or ModusToolbox IDE build claimed.

To advance status, attach exact SDK/compiler versions, the clean command and
log. `Bench-tested` additionally requires a real ADS1299 ID read, internal-test
waveform and packet integrity evidence. `24h-tested` requires loss/CRC/error
counts from a continuous 24-hour run.
