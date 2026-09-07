# Validation

Current status: **Reference**

Allowed lifecycle states: `Planned`, `Reference`, `Example`, `Compatible`,
`Compiles`, `Bench-tested`, `24h-tested`.

STM32U575 TrustZoneDisabled reference linked with actual startup and CubeU5 1.1.0 linker on 2026-09-06; zero unresolved symbols. Modeled faults, EOT delay and 65535+1 split passed. See build.md; no hardware or TrustZoneEnabled execution claimed.

To advance status, attach exact SDK/compiler versions, the clean command and
log. `Bench-tested` additionally requires a real ADS1299 ID read, internal-test
waveform and packet integrity evidence. `24h-tested` requires loss/CRC/error
counts from a continuous 24-hour run.
