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

- Official device, board, LPSPI route and SDK sources are in `sources.md`.
- `tests/validate_mimxrt1170.py` checks layering, metadata, pin documentation
  and syntax-compiles the adapter against a narrow MCUXpresso API fixture.
- An EVK-derived RAM reference ELF now links against SDK 2.16.000. See build.md
  and board/evk_reference.md. It does not validate EVKB wiring or hardware;
  the catalog retains **Reference** status.
