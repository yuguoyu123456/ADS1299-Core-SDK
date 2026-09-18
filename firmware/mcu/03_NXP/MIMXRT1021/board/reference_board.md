# MIMXRT1021 reference board

## Status

**TEMPLATE / NOT BUILD-VERIFIED / NOT BOARD-VERIFIED.**

This file defines the reference-board path for the MIMXRT1021 ADS1299 port. It does not claim that this repository has been compiled for, flashed to, or electrically verified on the board.

## Reference board

Use the **NXP MIMXRT1020-EVK** as the reference board for this model folder.

NXP documents this EVK with the **MIMXRT1021DAG5A** processor and MCUXpresso SDK support. The MCUXpresso SDK board directory/target name is **`evkmimxrt1020`**.

Official sources:

- NXP board page: https://www.nxp.com/design/design-center/development-boards-and-designs/MIMXRT1020-EVK
- MCUXpresso SDK board documentation: https://mcuxpresso.nxp.com/mcuxsdk/latest/html/boards/evkmimxrt1020/index.html

## One board-dependent configuration point

Keep ADS1299 behavior in the shared core. For a concrete MIMXRT1020-EVK project, bind only the hardware-facing callbacks used by `board_ads1299_hal()` / this model's `ads1299_port` layer:

- LPSPI instance and clock source
- SCK, MOSI and MISO pin mux
- software-controlled ADS1299 CS GPIO
- DRDY input GPIO/interrupt
- RESET, PWDN and START GPIOs
- microsecond delay/timebase
- optional non-blocking output transport

Do **not** edit `ads1299.c`, `ads1299_regs.h`, `ads1299_model.c` or other shared ADS1299 core files when changing boards.

The exact connector pins are intentionally not guessed here. Select exposed EVK pins from the official board schematic and configure those pins in the MCUXpresso project. Record the chosen assignment in `board/pinmap.md` before wiring hardware.

## MCUXpresso integration path

1. Install an MCUXpresso SDK release that contains board target `evkmimxrt1020`.
2. Import/create an `evkmimxrt1020` application using NXP's generated board/startup/linker content rather than copying invented startup files into this repository.
3. Enable one LPSPI peripheral in master mode and configure it for ADS1299 SPI Mode 1 (CPOL=0, CPHA=1), MSB first. Keep initial SCLK at or below 4 MHz during bring-up.
4. Configure CS as a normal GPIO and configure DRDY, RESET, PWDN and START as GPIOs.
5. Add this model folder's board/port/adapter sources plus the repository shared ADS1299 core to the application include/source paths.
6. Bind the MCUXpresso LPSPI/GPIO/time functions through the existing model HAL/port layer. Application code must call the shared ADS1299 API rather than writing register literals.
7. Build the vendor project first. Only after that succeeds should the status be promoted to BUILD-VERIFIED.

## Progressive ADS1299 bring-up

Use the examples in this order:

1. `examples/probe_id.c` — establish SPI communication and validate the ADS1299 ID path.
2. `examples/internal_test.c` — configure the shared internal-test profile and verify acquisition.
3. `examples/input_short.c` — validate the shorted-input acquisition path.
4. `examples/eeg_250sps.c` — run the shared 250-SPS normal-input EEG configuration.
5. `examples/bounded_stream.c` — acquire into a bounded queue and keep potentially blocking UART/USB/network work outside the DRDY timing path.

Do not connect human electrodes during initial digital bring-up. Complete ID, internal-test and input-short checks first.

## Diagnostics

Treat failures by layer rather than changing ADS1299 core code:

- **Configuration/port failure:** missing callback, invalid GPIO/peripheral selection, or MCUXpresso initialization failure. Fix the board/port binding.
- **SPI/ID failure:** check Mode 1, MSB-first, CS timing, SCLK, wiring and power before changing ADS1299 configuration.
- **DRDY timeout:** check START/RESET/PWDN state, DRDY wiring/interrupt setup and whether conversions were actually started.
- **Invalid state/config:** report the shared ADS1299 error and correct the typed configuration/profile call.
- **Stream overflow:** increment/report the bounded queue drop counter and improve the consumer/transport path; never block the DRDY acquisition path waiting for output.

## Same MCU, different board

For another board carrying MIMXRT1021, retain the shared ADS1299 core and the model examples. Change only the board-dependent LPSPI/GPIO/pin/clock/time/transport binding. A different board is not assumed pin-compatible with MIMXRT1020-EVK.
