# MCXA153 MCUXpresso adapter

Status: **TEMPLATE / NOT BUILD-VERIFIED**. This directory is an additive hardware adapter for an MCUXpresso SDK project; it does not replace or duplicate ADS1299 register logic from `firmware/core_driver/ads1299`.

Use an MCXA153 MCUXpresso SDK project that already initializes board clocks and pin mux. Add `ads1299_mcxa153_hal.c`, the existing `ads1299_port/*.c`, and the shared ADS1299 core sources. The adapter uses the MCUXpresso `fsl_lpspi.h` and `fsl_gpio.h` APIs.

Board code constructs one `ads1299_mcxa153_context_t`: select the real LPSPI instance, GPIO peripheral, source clock, SPI rate, five independent GPIO locations in the order CS/RESET/PWDN/START/DRDY, and a microsecond delay callback. Keep `spi_hz <= 4000000` during bring-up. Configure the physical SPI pins in the board pin-mux file and configure CS/RESET/PWDN/START as GPIO outputs and DRDY as an input. Do not use a guessed pin assignment.

The adapter requests SPI Mode 1 (`CPOL=0`, `CPHA=1` in ADS1299 terminology), MSB first and 8 bits per frame. CS remains a normal GPIO owned by the shared MCU port, so the LPSPI peripheral PCS output is not the ADS1299 chip-select contract.

A board project can provide the callback expected by `examples/main_ads1299.c` as follows:

```c
static ads1299_mcxa153_context_t ads_ctx = {
    .spi = LPSPI0, /* replace with the instance actually routed by your board */
    .gpio = GPIO,
    .source_hz = 0, /* fill from the configured LPSPI functional clock */
    .spi_hz = 1000000U,
    .poll_limit = 100000U,
    .delay_us = board_delay_us,
};

int board_ads1299_hal(ads1299_platform_hal_t *hal)
{
    /* Fill ads_ctx.ports[] and ads_ctx.pins[] from the verified board pin mux. */
    return ads1299_mcxa153_make_hal(&ads_ctx, hal);
}
```

The zero `source_hz` is intentional: the example must fail until the board project supplies its actual configured LPSPI clock. This prevents a template from pretending to be a verified board configuration.

Validation boundary: source/API integration has been added, but no claim is made that this adapter has compiled against a particular MCUXpresso SDK release or run on an MCXA153 board. The next validation step is an official MCXA153 reference-board project build, followed by ID/probe and internal-test hardware checks.
