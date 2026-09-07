# MCUXpresso adapter

This directory only bridges MCUXpresso SDK LPSPI/GPIO APIs to
`ads1299_platform_hal_t`. It does not initialize clock roots, pin muxes,
interrupts, or ADS1299 registers.

The consuming EVKB CM7 project initializes LPSPI1 as 8-bit, MSB-first, SPI
Mode 1 and initializes all GPIOs before calling
`ads1299_mcuxpresso_make_hal()`. Set `transfer_flags` to
`kLPSPI_MasterPcs0`; physical CS is driven by the generic Port through the D10
GPIO. No PCS pad may be muxed to LPSPI. Do not enable byte-swap flags.

Alternatively, after enabling the GPIO clocks and configuring pin mux/root
clocks, call `ads1299_mcuxpresso_init(hal, ctx, source_clock_hz, spi_clock_hz)`.
It initializes inactive control levels (CS/RESET/PWDN high, START low), DRDY
as input and LPSPI as 8-bit Mode 1 MSB-first. It rejects zero clocks, invalid
GPIO indices and duplicated control lines before changing hardware. Choose
the SPI rate using the Core timing limits and actual DVDD; MCU maximum SPI
speed is not the permissible ADS1299 speed. Both APIs require a persistent
context, initialized accurate `SystemCoreClock`, and exclusive peripheral
ownership; do not call from an interrupt while a transaction is in progress.

NXP's default `SPI_RETRY_TIMES=0` permits infinite polling. In the consuming
project define a finite `SPI_RETRY_TIMES` when compiling **fsl_lpspi.c itself**,
not just the adapter. SDK transfer errors propagate as -1; this is an iteration
limit, not a calibrated wall-clock timeout. Confirm the bound on the target.
After a transfer error, stop acquisition and reinitialize the peripheral/Core
before retrying; do not treat a partially received frame as valid.

## Board binding contract

The minimal example deliberately requires the consuming board to implement
`int board_ads1299_hal(ads1299_platform_hal_t *hal)`. In that function:

1. Initialize the SDK system clocks and refresh `SystemCoreClock`.
2. Enable GPIO clocks and apply Pins-tool output for the actual PCB revision.
3. Fill a **static** `ads1299_mcuxpresso_ctx_t` with LPSPI1 and five distinct
   GPIO base/pin pairs. Pin is a numeric index 0..31, not a bit mask.
4. Query `CLOCK_GetRootClockFreq(kCLOCK_Root_Lpspi1)` after root setup and pass
   the result and the chosen SPI frequency to `ads1299_mcuxpresso_init()`.
5. Return that function's result. Do not add chip register writes here.

The ADS1299 test-signal configuration and frame handling remain in the example
and Core. An optional complete binding exists in `board/evk_reference.c` for
the separately documented EVK-derived profile, not verified EVKB wiring.
The example outputs a latest-frame debugger snapshot. UART/USB output is still
a consumer-supplied integration step; it is not complete streaming firmware.
