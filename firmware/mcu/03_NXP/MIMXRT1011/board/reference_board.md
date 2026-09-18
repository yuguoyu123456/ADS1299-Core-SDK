# MIMXRT1011 reference board

## Reference hardware

Use the **NXP MIMXRT1010-EVK** as the reference development board for this model folder.
NXP documents this board as using the **MIMXRT1011DAE5A** (80-pin LQFP) and identifies
`evkmimxrt1010` as the MCUXpresso SDK board target. The EVK provides an Arduino UNO R3
expansion site, an on-board LPC-Link2 debug probe, and a virtual COM port.

Official sources:

- NXP MIMXRT1010-EVK product page: https://www.nxp.com/design/design-center/development-boards-and-designs/i-mx-evaluation-and-development-boards/i-mx-rt1010-evaluation-kit:MIMXRT1010-EVK
- MCUXpresso SDK board documentation: `boards/evkmimxrt1010`

## What the student edits

Keep ADS1299 behavior in the shared core. Board-specific choices belong in the consuming
MCUXpresso project and the model-folder board/HAL layer only:

1. choose one free LPSPI instance and route SCK/MOSI/MISO;
2. choose GPIOs for CS, DRDY, RESET, PWDN and START;
3. configure the LPSPI peripheral for ADS1299 SPI Mode 1 (CPOL=0, CPHA=1), MSB first;
4. bind those SDK handles/GPIO operations to `board_ads1299_hal()`;
5. keep CS software-controlled and keep transport output outside the DRDY timing path.

Do **not** edit `firmware/core_driver/ads1299/` for board pin changes.

`pinmap.md` intentionally does not invent header pins. Before wiring, select pins that are
actually routed to the EVK headers and verify their mux functions against the exact EVK
revision schematic and the MCUXpresso Config Tools pin view. Record the chosen mapping in
the application project.

## MCUXpresso project starting point

Create or import an SDK project for board target `evkmimxrt1010`, then add this repository's
shared ADS1299 core plus this model folder's port/board integration. Use the existing examples
in this folder in progression:

1. `examples/probe_id.c`
2. `examples/internal_test.c`
3. `examples/input_short.c`
4. `examples/eeg_250sps.c`
5. `examples/bounded_stream.c`

The existing `integration.md` describes the repository-side source integration contract.
Do not copy ADS1299 register definitions into the MCUXpresso project.

## Expected first success

The first hardware milestone is a successful ADS1299 family ID/probe through the shared
driver. Only after that should the internal-test, input-short and 250-SPS EEG flows be used.
A DRDY timeout should be treated separately from an SPI/ID failure; transport queue drops
are a streaming/backpressure diagnostic, not an ADS1299 register error.

## Validation status

**TEMPLATE / NOT BUILD-VERIFIED / NOT BOARD-VERIFIED.**

The reference-board identity and SDK board target are based on NXP documentation. This
repository has not yet demonstrated a successful documented MCUXpresso build for this leaf
and no physical MIMXRT1010-EVK + ADS1299 run is claimed here.
