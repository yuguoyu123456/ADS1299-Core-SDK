# Validation

Current status: **Reference**

Allowed lifecycle states: `Planned`, `Reference`, `Example`, `Compatible`,
`Compiles`, `Bench-tested`, `24h-tested`.

Complete 17-source Core0 ELF linked with MCUX_2.16.000 on 2026-09-07, vectors at zero and no unresolved symbols. Adapter API-double and real vendor SPI / modeled FIFO tests passed. See build.md. Provisional control pins and untested boot/hardware keep status Reference.

To advance status, attach exact SDK/compiler versions, the clean command and
log. `Bench-tested` additionally requires a real ADS1299 ID read, internal-test
waveform and packet integrity evidence. `24h-tested` requires loss/CRC/error
counts from a continuous 24-hour run.
