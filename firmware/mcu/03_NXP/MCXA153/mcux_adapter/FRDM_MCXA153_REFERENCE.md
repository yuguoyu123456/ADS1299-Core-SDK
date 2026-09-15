# FRDM-MCXA153 reference-board path

Status: **REFERENCE BOARD SELECTED / TEMPLATE / NOT BUILD-VERIFIED / NOT BOARD-VERIFIED**.

The recommended first board for this model folder is the official **NXP FRDM-MCXA153**. NXP documents this board for MCXA153 and provides an on-board MCU-Link debugger plus Arduino-compatible headers. This file fixes the SPI side of the ADS1299 wiring to a documented board connector; it deliberately does not invent ADS1299 control GPIO assignments that have not been validated on hardware.

## Documented SPI route

NXP FRDM-MCXA153 Board User Manual UM12012 documents LPSPI1 on the Arduino header:

| ADS1299 signal | FRDM-MCXA153 Arduino signal | MCU signal |
|---|---|---|
| SCLK | D13 | P2_12 / LPSPI1_SCK |
| DOUT -> MCU | D12 | P2_16 / LPSPI1_SDI |
| DIN <- MCU | D11 | P2_13 / LPSPI1_SDO |
| CS | do **not** use peripheral PCS as the SDK CS contract | select a verified GPIO below |

The board also exposes D10 as P2_6/LPSPI1_PCS1, but this SDK intentionally owns ADS1299 CS as a normal GPIO so shared-core transaction boundaries remain portable and multi-ADS1299 systems can use independent CS lines. A board project may mux a suitable header pin as GPIO for CS; do not silently substitute hardware PCS behavior.

## Control signals still requiring a board choice

Before hardware use, select and verify five GPIO-capable header locations for `CS`, `RESET`, `PWDN`, `START`, and `DRDY`. Configure the first four as outputs and DRDY as an input. Record those choices in the board project's pin-mux/config file and in the `ads1299_mcxa153_context_t` `ports[]` / `pins[]` arrays. The repository template intentionally contains no guessed values.

This is the only board-specific information a beginner should need to change: pin mux/GPIO locations, the LPSPI1 functional source clock reported by the MCUXpresso clock configuration, and (if needed) the bring-up SPI rate. Shared ADS1299 register/model/core files must not be edited.

## MCUXpresso project starting point

1. Create/import an official FRDM-MCXA153 MCUXpresso SDK project and first prove that it builds/flashes through the on-board MCU-Link.
2. Enable/configure LPSPI1 using the documented D11/D12/D13 route and configure five selected ADS1299 control GPIOs.
3. Add `mcux_adapter/ads1299_mcxa153_hal.c`, the existing `ads1299_port` sources, and the shared `firmware/core_driver/ads1299` sources required by the example.
4. Set the adapter `spi` to the LPSPI1 instance, set `source_hz` from the actual configured LPSPI1 functional clock, keep initial `spi_hz <= 4000000`, fill the five GPIO locations, and provide the microsecond-delay callback.
5. Build before connecting ADS1299. Then proceed to probe/ID and the repository's progressive ADS1299 examples.

## Validation boundary

The board identity and LPSPI1 Arduino-header route above come from NXP's FRDM-MCXA153 documentation. The repository has **not** yet compiled this adapter inside a downloaded FRDM-MCXA153 MCUXpresso SDK project and has **not** run it on physical FRDM-MCXA153 + ADS1299 hardware. Therefore this remains TEMPLATE, not BUILD-VERIFIED or BOARD-VERIFIED.

Primary board references: NXP FRDM-MCXA153 product page and NXP UM12012 FRDM-MCXA153 Board User Manual, especially the LPSPI interface and Arduino socket connector sections.