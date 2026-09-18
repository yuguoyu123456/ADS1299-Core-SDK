# MIMXRT1042 reference board

## Reference target

Use the **NXP MIMXRT1040-EVK** as the reference development board for this model folder.
The board carries a MIMXRT1042 device and is supported by NXP's MCUXpresso SDK.  The
MCUXpresso board target is `evkmimxrt1040`.

This file selects the reference board; it does **not** claim that the ADS1299 wiring in
this repository has been built or exercised on physical hardware.

## Validation status

- Repository integration: **TEMPLATE / PLANNED**
- MCUXpresso reference-target compile: **NOT BUILD-VERIFIED**
- Physical MIMXRT1040-EVK + ADS1299: **NOT BOARD-VERIFIED**

Do not upgrade these labels until the corresponding evidence exists.

## What belongs in the board layer

For an `evkmimxrt1040` application, keep all hardware-dependent choices here or in the
application's board/config binding:

- LPSPI instance, source clock and baud rate;
- LPSPI SCK/MOSI/MISO pin mux generated/selected with MCUXpresso tools;
- software-controlled ADS1299 CS GPIO;
- ADS1299 DRDY input;
- RESET, PWDN and START GPIO outputs;
- microsecond timebase/delay implementation;
- UART/USB/network transport selection used after acquisition buffering.

The shared ADS1299 core must not be edited when these choices change.  A different board
using the same MIMXRT1042 should normally replace only this board/config binding and its
vendor-generated pin/clock setup.

## Reference bring-up order

Start from an NXP MCUXpresso SDK project for board `evkmimxrt1040` rather than inventing
startup code, linker files or clock initialization in this repository.  Add this model
folder's `ads1299_port/` and `mcux_adapter/` sources plus the shared ADS1299 core, then
bind the real board pins/clocks in the board layer.

Run the examples in this order:

1. `examples/probe_id.c`
2. `examples/internal_test.c`
3. `examples/input_short.c`
4. `examples/eeg_250sps.c`
5. `examples/bounded_stream.c`

A successful hardware bring-up must begin with a valid ADS1299-family ID before the
acquisition examples are trusted.  Streaming transport must consume from the bounded
buffer rather than blocking the DRDY acquisition path.

## Hardware rules

ADS1299 SPI is Mode 1 (CPOL=0, CPHA=1), MSB first.  Keep CS under GPIO control.  Do not
copy ADS1299 register setup into the board layer; use the shared typed/profile APIs.
Confirm the exact expansion-header pins against the MIMXRT1040-EVK revision and the
MCUXpresso-generated pin configuration before connecting hardware.

## Authoritative sources

- NXP MIMXRT1040-EVK product page: https://www.nxp.com/design/design-center/development-boards-and-designs/MIMXRT1040-EVK
- NXP MCUXpresso SDK board documentation: board `evkmimxrt1040`, device MIMXRT1042.
- Texas Instruments ADS1299-family datasheet for ADS1299 electrical and SPI behavior.
