# Validation

Current status: **Reference**

Allowed lifecycle states: `Planned`, `Reference`, `Example`, `Compatible`,
`Compiles`, `Bench-tested`, `24h-tested`.

A real FSP 5.6.0 reference ELF linked on 2026-09-05 with zero undefined symbols; see build.md. Physical execution and FSP 6.5 BSP build are unverified.

To advance status, attach exact SDK/compiler versions, the clean command and
log. `Bench-tested` additionally requires a real ADS1299 ID read, internal-test
waveform and packet integrity evidence. `24h-tested` requires loss/CRC/error
counts from a continuous 24-hour run.

## Rank 8 software reference gate, 2026-09-05

FSP 5.6 complete reference ELF passed: 25 sources and zero undefined symbols, without fixture APIs. Separately, ARM-emulated adapter tests cover faults and frozen timer. No physical runtime test claimed. See build.md. The software gate permits moving to rank 9; catalog remains conservatively Reference.
