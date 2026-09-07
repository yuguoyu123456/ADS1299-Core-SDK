# Authoritative sources

Checked: 2026-09-05.

- TI MSPM0G3507 product page and MSPM0G350x datasheet.
- TI LP-MSPM0G3507 User's Guide Rev. D (SLAU873D), including the 40-pin
  BoosterPack map and on-board XDS110 details.
- TI MSPM0 SDK 2.11.00.07 Git repository, manifest and LP-MSPM0G3507 SPI
  controller examples.
- TI DriverLib SPI/GPIO/Common API documentation.

No TI SDK source is redistributed. The adapter is original repository code
written against documented DriverLib entry points.

## Pinned build inputs

[TI SDK revision used](https://github.com/TexasInstruments/mspm0-sdk/tree/20807db79aa17b49f87ab8ec87f6b6d63ee2cb32)
is SDK 2.11.00.07. The source headers for DriverLib, device definitions and
startup/linker have BSD-style three-clause notices; they stay in the external
SDK installation with their notices intact. No TI Commercial components or
example implementation were copied into this project. The board binding is
original code using public API and pin facts.

Reference API/clock/SPI setup was checked against the SDK's
`examples/nortos/LP_MSPM0G3507/driverlib/spi_controller_multibyte_fifo_poll`.
Its default Mode 0 and hardware-CS setup are not used: this project selects
Mode 1 and GPIO CS. The device header maps PB6 to PINCM23, PA25 to PINCM55,
PA26 to PINCM59, PA27 to PINCM60 and PA28 to PINCM3. Header connector positions
and actual acquisition remain hardware checks, not software-build evidence.
