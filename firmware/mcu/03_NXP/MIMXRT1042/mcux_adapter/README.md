# MIMXRT1042 MCUXpresso ADS1299 adapter

Status: **TEMPLATE / NOT BUILD-VERIFIED / NOT BOARD-VERIFIED**.

This directory binds the existing MIMXRT1042 `ads1299_port` abstraction to real MCUXpresso SDK LPSPI and GPIO APIs. ADS1299 register setup, profiles, frame parsing, self-test and multi-device behavior remain in the shared core/platform layers; do not copy them here.

## What the board project supplies

Create one `ads1299_mimxrt1042_hal_context_t` in the board/application layer and set:

- the MCUXpresso `LPSPI_Type *` instance;
- its actual source clock in Hz;
- an initial ADS1299 SPI baud rate (the adapter rejects values above 4 MHz for conservative bring-up);
- GPIO controller plus pin number for CS, RESET, PWDN, START and DRDY;
- a working microsecond delay callback.

Pin mux, clocks and GPIO direction/pad setup remain the responsibility of the MCUXpresso board project. Do not guess EVK pins: take them from the exact board schematic and generated/configured MCUXpresso pin project.

## Integration sequence

1. Add `ads1299_mimxrt1042_hal.c` to the MCUXpresso application together with this model's existing `ads1299_port` sources and the repository shared ADS1299 core required by the examples.
2. Include `ads1299_mimxrt1042_hal.h` from the board/application configuration file.
3. Initialize board clocks, IOMUXC/pins and GPIO directions first. CS, RESET, PWDN and START are outputs; DRDY is an input.
4. Fill the context and call `ads1299_mimxrt1042_hal_init()`. A non-zero result is a configuration/SDK initialization failure and must be fixed before probing ADS1299.
5. Call `ads1299_mimxrt1042_make_hal()` and pass the returned `ads1299_platform_hal_t` to the existing MIMXRT1042 port initialization path.
6. Run the model's progressive examples: probe/ID, internal test, input short, 250-SPS EEG, then streaming.

The adapter configures LPSPI as ADS1299 SPI Mode 1 (CPOL=0, CPHA=1), MSB-first, 8-bit transfers. CS remains a software-controlled GPIO through the existing platform HAL; the LPSPI transfer itself does not replace the repository CS callback.

## Failure codes

`ads1299_mimxrt1042_hal_init()` returns:

- `-1`: missing LPSPI/source clock/delay, zero baud, or baud above the bring-up limit;
- `-2`: missing/invalid required GPIO;
- `-3`: two ADS1299 control signals were assigned to the same GPIO;
- `-4`: MCUXpresso `LPSPI_MasterInit()` failed.

SPI transfer failures are reported through the existing HAL callback as `-1`; DRDY read failure also returns `-1` through the pin-read callback.

## Validation boundary

This adapter uses the MCUXpresso SDK LPSPI/GPIO API shape used by i.MX RT devices, but this repository change alone does **not** prove that an MIMXRT1042 EVK project compiles, that a particular pin map is correct, or that ADS1299 hardware has been exercised. Upgrade the status to BUILD-VERIFIED only after a documented MCUXpresso reference configuration compiles successfully, and to BOARD-VERIFIED only after physical-board evidence exists.
