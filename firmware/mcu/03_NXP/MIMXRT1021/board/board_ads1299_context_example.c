/*
 * MIMXRT1020-EVK / MIMXRT1021 ADS1299 board-context template.
 *
 * TEMPLATE / NOT BUILD-VERIFIED / NOT BOARD-VERIFIED.
 *
 * Copy this file into the MCUXpresso evkmimxrt1020 application (or add it to
 * that target) and replace only the BOARD_ADS1299_* macros below with symbols
 * from the application's generated pin/clock configuration.  Do not edit the
 * shared ADS1299 core for board bring-up.
 */
#include "../mcux_adapter/ads1299_mimxrt1021_hal.h"

#ifndef BOARD_ADS1299_LPSPI
#error "Define BOARD_ADS1299_LPSPI to the MCUXpresso LPSPI instance"
#endif
#ifndef BOARD_ADS1299_LPSPI_CLOCK_HZ
#error "Define BOARD_ADS1299_LPSPI_CLOCK_HZ to the actual LPSPI source clock"
#endif
#ifndef BOARD_ADS1299_SPI_BAUD_HZ
#define BOARD_ADS1299_SPI_BAUD_HZ 1000000u
#endif
#ifndef BOARD_ADS1299_GPIO
#error "Define BOARD_ADS1299_GPIO to the GPIO peripheral used by ADS1299 control pins"
#endif
#ifndef BOARD_ADS1299_CS_PIN
#error "Define BOARD_ADS1299_CS_PIN"
#endif
#ifndef BOARD_ADS1299_RESET_PIN
#error "Define BOARD_ADS1299_RESET_PIN"
#endif
#ifndef BOARD_ADS1299_PWDN_PIN
#error "Define BOARD_ADS1299_PWDN_PIN"
#endif
#ifndef BOARD_ADS1299_START_PIN
#error "Define BOARD_ADS1299_START_PIN"
#endif
#ifndef BOARD_ADS1299_DRDY_PIN
#error "Define BOARD_ADS1299_DRDY_PIN"
#endif

/* Provide this with an MCUXpresso timer/timebase; it must be safe during bring-up. */
extern void board_ads1299_delay_us(uint32_t us);

static ads1299_mimxrt1021_hal_context_t g_ads1299_context = {
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

ads1299_mimxrt1021_hal_context_t *board_ads1299_context(void)
{
    return &g_ads1299_context;
}
