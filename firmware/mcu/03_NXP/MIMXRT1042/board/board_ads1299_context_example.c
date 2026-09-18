/*
 * MIMXRT1040-EVK / MIMXRT1042 ADS1299 board-context template.
 *
 * TEMPLATE / NOT BUILD-VERIFIED / NOT BOARD-VERIFIED.
 *
 * Add this file to an MCUXpresso evkmimxrt1040 application. Hardware-dependent
 * choices live in board_ads1299_config.h (or equivalent build definitions),
 * keeping the shared ADS1299 core and this adapter glue unchanged when moving
 * to another MIMXRT1042 board.
 */
#include "board_ads1299_config.h"
#include "../mcux_adapter/ads1299_mimxrt1042_hal.h"

/* Provide this from the application timebase; it must be safe during bring-up. */
extern void board_ads1299_delay_us(uint32_t us);

static ads1299_mimxrt1042_hal_context_t g_ads1299_context = {
    .lpspi = BOARD_ADS1299_LPSPI,
    .source_clock_hz = BOARD_ADS1299_LPSPI_CLOCK_HZ,
    .baud_hz = BOARD_ADS1299_SPI_BAUD_HZ,
    .cs = { BOARD_ADS1299_GPIO, BOARD_ADS1299_CS_PIN },
    .reset = { BOARD_ADS1299_GPIO, BOARD_ADS1299_RESET_PIN },
    .pwdn = { BOARD_ADS1299_GPIO, BOARD_ADS1299_PWDN_PIN },
    .start = { BOARD_ADS1299_GPIO, BOARD_ADS1299_START_PIN },
    .drdy = { BOARD_ADS1299_GPIO, BOARD_ADS1299_DRDY_PIN },
    .delay_us = board_ads1299_delay_us,
};

ads1299_mimxrt1042_hal_context_t *board_ads1299_context(void)
{
    return &g_ads1299_context;
}
