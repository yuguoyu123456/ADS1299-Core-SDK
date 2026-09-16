# LPC54628 ADS1299 progressive examples

Validation status: **TEMPLATE / NOT BUILD-VERIFIED / NOT BOARD-VERIFIED**.

These files deliberately keep ADS1299 register semantics in the shared core. The application supplies `board_ads1299_hal()` from the LPC54628 board/config integration; a new student should not edit `firmware/core_driver/ads1299/`.

Recommended bring-up order:

1. `probe_id.c` — reset, leave continuous mode, read and validate the ADS1299-family ID. Exit 10-12 indicates board/port setup, 20-21 reset/control, and 30-31 SPI/ID failure.
2. `internal_test.c` — use the shared internal-test configuration and inspect acquired frames.
3. `input_short.c` — route channels to the shared input-short configuration and inspect converter baseline/noise.
4. `main_ads1299.c` — existing combined continuous-acquisition example.

Each file is an alternative application `main()`, not an additional translation unit to link beside another example main. Keep UART/USB/network formatting out of the DRDY-critical acquisition section; copy frames into a bounded application buffer before slower transport work.

Still missing before this leaf is candidate-complete: a dedicated typed 250-SPS EEG example, a bounded streaming example, and a real MCUXpresso reference-board build path. These examples do not imply hardware validation.
