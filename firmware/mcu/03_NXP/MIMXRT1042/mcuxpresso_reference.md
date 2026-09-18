# MIMXRT1042 MCUXpresso reference integration

## Validation status

**TEMPLATE / NOT BUILD-VERIFIED / NOT BOARD-VERIFIED.**

This recipe deliberately uses the official **MIMXRT1040-EVK** / MCUXpresso board target
`evkmimxrt1040`. NXP's current SDK documentation identifies the device on that target as
MIMXRT1042 (part MIMXRT1042XJM5B). The vendor SDK owns startup, linker, clock, pin-mux and
debug files; do not copy guessed versions into this repository.

## Start from a real vendor project

1. Install/import the MCUXpresso SDK package containing board `evkmimxrt1040`.
2. Import the vendor `demo_apps/hello_world` example for `evkmimxrt1040` and build it
   unchanged first. This establishes that the SDK/toolchain/debug setup is healthy.
3. Keep the generated/vendor project outside this repository. Add the ADS1299 sources to
   that application rather than replacing vendor startup or linker files.

## ADS1299 source set to add

Add the shared ADS1299 core sources required by the selected example from:

- `firmware/core_driver/ads1299/`

Add this model's integration sources:

- `firmware/mcu/03_NXP/MIMXRT1042/ads1299_port/ads1299_port.c`
- `firmware/mcu/03_NXP/MIMXRT1042/mcux_adapter/ads1299_mimxrt1042_mcux.c`
- `firmware/mcu/03_NXP/MIMXRT1042/board/board_ads1299.c`
- one application-owned implementation derived from
  `firmware/mcu/03_NXP/MIMXRT1042/board/board_ads1299_context_example.c`
- exactly one beginner example source from `examples/` as the application's main flow
  (start with `probe_id.c`).

Add include paths for the shared core and the three MIMXRT1042 integration directories
above. Do not copy ADS1299 register definitions into the MCUXpresso application.

## The only board-specific work

In the application-owned board context, bind:

- the LPSPI instance used for ADS1299;
- the **actual** LPSPI source-clock frequency supplied by the vendor clock tree;
- software-controlled CS GPIO;
- DRDY input GPIO;
- RESET, PWDN and START output GPIOs;
- a microsecond delay callback.

Configure the chosen LPSPI pins with MCUXpresso Config Tools or the vendor project's
pin-mux source. The ADS1299 adapter configures the peripheral for SPI Mode 1
(CPOL=0, CPHA=1), MSB first; keep CS under the separate GPIO callback. Begin at the
repository context template's conservative 1 MHz SPI baud unless the application has a
verified reason to change it.

There are intentionally no hard-coded EVK connector pins in this recipe: the exact
connector/pin choice belongs to the consuming project's pin-mux and must be checked
against the board revision/schematic. This avoids presenting an invented wiring map as a
verified reference design.

## Bring-up order

Build and run these flows one at a time:

1. `examples/probe_id.c` -- first required hardware checkpoint;
2. `examples/internal_test.c`;
3. `examples/input_short.c`;
4. `examples/eeg_250sps.c`;
5. `examples/bounded_stream.c`.

Do not connect electrodes until power rails, SPI/ID and internal/input-short validation
have succeeded. A successful software build is only **BUILD-VERIFIED**; it does not make
this integration **BOARD-VERIFIED**. Record the exact MCUXpresso SDK/toolchain version
when a real compile is performed.

## Porting to another MIMXRT1042 board

Keep the shared ADS1299 core, model port, MCUX adapter and examples unchanged. Replace
only the application-owned board context/pin-mux/clock bindings (and transport plumbing
if desired). If a port requires editing `ads1299.c`, `ads1299_regs.h` or model/register
logic, treat that as an integration defect rather than normal board configuration.
