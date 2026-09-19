/*
 * MIMXRT1052 ADS1299 board-context template.
 *
 * TEMPLATE / NOT BUILD-VERIFIED / NOT BOARD-VERIFIED.
 *
 * Add this file to the consuming MCUXpresso application. All hardware-dependent
 * choices remain in board_ads1299_config.h (or equivalent compiler definitions),
 * so changing to another MIMXRT1052 board does not require edits to the shared
 * ADS1299 core or the MCUX adapter.
 */
#include "board_ads1299_config.h"
#include "../mcux_adapter/ads1299_mimxrt1052_hal.h"

static ads1299_mimxrt1052_hal_context_t g_ads1299_context = {
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

ads1299_mimxrt1052_hal_context_t *board_ads1299_context(void)
{
    return &g_ads1299_context;
}
