# MCXA156 MCUXpresso ADS1299 adapter

Status: **TEMPLATE / not build-verified / not board-verified**.

This directory is the MCXA156-specific bridge between the existing model-folder `ads1299_port` layer and NXP MCUXpresso SDK drivers. ADS1299 register behavior remains in the repository shared core; do not copy register setup into this adapter.

## Files

- `ads1299_mcxa156_hal.h` — one context structure for the selected LPSPI instance, GPIO block, five ADS1299 control/data-ready GPIOs, clocks, and delay callback.
- `ads1299_mcxa156_hal.c` — MCUXpresso LPSPI/GPIO implementation of `ads1299_platform_hal_t`.

The five GPIO entries use the existing platform-pin order: `CS`, `RESET`, `PWDN`, `START`, `DRDY`.

## Board-owned configuration

Keep hardware choices in board-generated or board-specific code. Configure the chosen LPSPI clock/root and pin mux, configure CS/RESET/PWDN/START as GPIO outputs, configure DRDY as an input, then populate `ads1299_mcxa156_context_t`. Do not edit shared ADS1299 core files for a new board.

The adapter rejects missing SPI/GPIO handles, zero source/SPI clocks, a missing delay callback, invalid GPIO pin indices, duplicate ADS1299 GPIO assignments, and bring-up SPI rates above 4 MHz. It initializes LPSPI for SPI Mode 1 (CPOL=0, CPHA=1), MSB-first, 8-bit frames. CS remains software-controlled through the existing port contract.

## Integration skeleton

```c
#include "ads1299_mcxa156_hal.h"

static void board_delay_us(void *user, uint32_t us)
{
    (void)user;
    /* Call the board's real microsecond delay implementation here. */
}

ads1299_mcxa156_context_t hw = {
    .spi = BOARD_ADS1299_LPSPI,
    .gpio = GPIO,
    .ports = { BOARD_CS_PORT, BOARD_RESET_PORT, BOARD_PWDN_PORT,
               BOARD_START_PORT, BOARD_DRDY_PORT },
    .pins = { BOARD_CS_PIN, BOARD_RESET_PIN, BOARD_PWDN_PIN,
              BOARD_START_PIN, BOARD_DRDY_PIN },
    .source_hz = BOARD_ADS1299_LPSPI_SOURCE_HZ,
    .spi_hz = 1000000U,
    .poll_limit = 100000U,
    .delay_us = board_delay_us,
};

ads1299_platform_hal_t hal;
if (ads1299_mcxa156_make_hal(&hw, &hal) != 0) {
    /* Configuration/port failure: stop before touching ADS1299 core. */
}
```

`BOARD_*` names above are intentionally placeholders, not invented MCXA156 SDK symbols. Replace them with values/symbols from the actual MCUXpresso project generated for the chosen board.

## Validation boundary

This adapter has not yet been compiled inside an official MCXA156 MCUXpresso SDK project and has not been run on physical hardware. Repository CI success elsewhere must not be interpreted as MCXA156 BUILD-VERIFIED or BOARD-VERIFIED evidence. The next maturity step is an official reference-board project that compiles this adapter together with the model port and shared ADS1299 core.
