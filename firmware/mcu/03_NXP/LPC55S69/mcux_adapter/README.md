# LPC55S69 Core0 MCUX adapter

Status: Reference. This is a hardware-only SPI/GPIO adapter. ADS1299 register,
command and frame handling remain in the shared Core. This directory is not a
standalone bootable board project.

## Dependency and ownership

Verified against the official NXP mcux-sdk tag `MCUX_2.16.000`, commit
`6f3fd257cdcf978a4d26e7d6e9eed9240037422b`, using
`CPU_LPC55S69JBD100_cm33_core0`. The locally ignored sparse checkout is under
`../tests/.tools/mcux-sdk`. Upstream: https://github.com/nxp-mcuxpresso/mcux-sdk
The selected Flexcomm SPI files carry BSD-3-Clause notices; retain the original
notices and consult the SDK content register for other components. No upstream
driver file was rewritten or shipped as an original ADS1299 implementation.

The caller configures clocks, pin multiplexing, GPIO output directions and DRDY
input before `ads1299_lpc55_make_hal`. Supply the actual Flexcomm source frequency,
an appropriate SPI frequency, a nonzero polling budget and a working microsecond
delay callback. GPIO arrays use CS, RESET, PWDN, START, DRDY order. Do not connect
the hardware SSEL output to the ADS1299 CS net; the shared Core owns physical CS.
Use one exclusive owner for the whole Core transaction, not a lock per chunk.

The adapter sets MSB-first, 8-bit SPI mode 1 and rounds the divider upward so the
requested clock is a ceiling. It preloads CS/RESET/PWDN high and START low.
This does not substitute for safe GPIO state during boot before initialization.

## Finite waits and fault handling

Compile **both** this adapter and the actual `fsl_spi.c` with
`SPI_RETRY_TIMES=100000` (or another reviewed finite value). The SDK default zero
means unbounded waits. Do not link a precompiled driver with an unknown setting.
The adapter additionally waits for MSTIDLE with `poll_limit`, checks FIFO errors
and rejects stale RX data. Poll counts are not calibrated time durations.

Transfers split into at most 32-byte synchronous chunks without toggling GPIO CS.
On failure SPI is disabled and the context fault is latched. GPIO writes remain
available so the Core can release CS. DRDY reads and subsequent transfers fail.
Discard partial receive data. Recovery requires board-controlled reinitialization
after diagnosing the cause; do not simply clear the fault while hardware is active.

## Software evidence, 2026-09-07

`../tests/compile_adapter.py --sdk PATH --cmsis PATH --cc PATH` compiled ten
objects with real NXP headers, including the real vendor SPI source and shared
Core, with Arm GCC 9.2.1. The SDK GetCount unused-parameter warning remains
visible; only that warning category on the vendor source is nonfatal.

`../tests/run_adapter_tests.py --sdk PATH --cmsis PATH --cc PATH` requires
Unicorn and pyelftools. It executes adapter code with genuine SDK types but
test-double SPI functions on an ARM emulator. It passed 65-byte chunking,
CS ownership, nullable buffers, invalid arguments, SPI API failure, bounded
non-idle failure, FIFO/stale RX rejection, latched faults and safe CS release.
This does not execute the real SDK SPI driver or model LPC55 hardware timing.

Update 2026-09-07: the cold-boot reference binding and full ELF link now pass,
as do real fsl_spi.c tests with modeled FIFO behavior. See ../build.md and
../board/reference_image.md. The reference software gate is met; board routing,
physical boot and 24-hour acquisition remain unverified. Retain Reference status.
