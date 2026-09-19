/*
 * MIMXRT1052 ADS1299 board-contract compile smoke target.
 *
 * TEMPLATE / NOT BUILD-VERIFIED / NOT BOARD-VERIFIED.
 *
 * Add this translation unit to the consuming MCUXpresso project while doing
 * first integration.  A successful compile proves that the application has
 * supplied the required board configuration macros and that those definitions
 * are type-compatible with the MIMXRT1052 ADS1299 HAL context.  It does not
 * imply that the board has been flashed or that ADS1299 hardware was tested.
 */
#include "board_ads1299_config.h"
#include "../mcux_adapter/ads1299_mimxrt1052_hal.h"

/* Keep the checks in ordinary C expressions so the vendor compiler validates
 * the same types that board_ads1299_context_example.c consumes. */
static ads1299_mimxrt1052_hal_context_t g_ads1299_config_compile_check = {
    .lpspi = BOARD_ADS1299_LPSPI,
    .source_clock_hz = BOARD_ADS1299_LPSPI_SOURCE_CLOCK_HZ,
    .baud_hz = BOARD_ADS1299_SPI_BAUD_HZ,
    .cs = { BOARD_ADS1299_CS_GPIO, BOARD_ADS1299_CS_PIN },
    .reset = { BOARD_ADS1299_RESET_GPIO, BOARD_ADS1299_RESET_PIN },
    .pwdn = { BOARD_ADS1299_PWDN_GPIO, BOARD_ADS1299_PWDN_PIN },
    .start = { BOARD_ADS1299_START_GPIO, BOARD_ADS1299_START_PIN },
    .drdy = { BOARD_ADS1299_DRDY_GPIO, BOARD_ADS1299_DRDY_PIN },
    .delay_us = BOARD_ADS1299_DELAY_US,
};

/* Reference the object from a callable symbol to avoid unused-static warnings
 * in projects that promote warnings to errors. */
ads1299_mimxrt1052_hal_context_t *board_ads1299_config_compile_check(void)
{
    return &g_ads1299_config_compile_check;
}
