# Validation

Current status: **Reference**

Allowed lifecycle states: `Planned`, `Reference`, `Example`, `Compatible`,
`Compiles`, `Bench-tested`, `24h-tested`.

A real SDK 2.16.000 RAM reference ELF linked on 2026-09-05; see build.md for exact scope. Hardware execution is unverified.

To advance status, attach exact SDK/compiler versions, the clean command and
log. `Bench-tested` additionally requires a real ADS1299 ID read, internal-test
waveform and packet integrity evidence. `24h-tested` requires loss/CRC/error
counts from a continuous 24-hour run.

## Evidence boundary

- `tests/validate_mimxrt1062.py` checks the official header mapping, layering,
  metadata and adapter syntax against a narrow SDK API fixture.
- A complete RAM reference ELF now links against SDK 2.16.000 with zero
  undefined symbols. See build.md and tests/acceptance.md. Hardware capture
  remains unverified; the catalog conservatively retains **Reference**.
