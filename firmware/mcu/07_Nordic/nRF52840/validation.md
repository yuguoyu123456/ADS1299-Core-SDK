# Validation

Current status: **Compiles**

Allowed lifecycle states: `Planned`, `Reference`, `Example`, `Compatible`,
`Compiles`, `Bench-tested`, `24h-tested`.

Build entrypoint: `west build -p always --no-sysbuild -b nrf52840dk/nrf52840 firmware/mcu/07_Nordic/nRF52840`. A pinned CI build definition is retained; it was not rerun locally in this maintenance session.

To advance status, attach exact SDK/compiler versions, the clean command and
log. `Bench-tested` additionally requires a real ADS1299 ID read, internal-test
waveform and packet integrity evidence. `24h-tested` requires loss/CRC/error
counts from a continuous 24-hour run.
