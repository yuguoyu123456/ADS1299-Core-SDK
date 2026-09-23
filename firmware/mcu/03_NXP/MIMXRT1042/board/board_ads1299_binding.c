#include "board_ads1299_binding.h"
#include "board_ads1299_config.h"
#include "../mcux_adapter/ads1299_mimxrt1042_hal.h"

/* Application-owned bring-up-safe microsecond timebase. */
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

int board_ads1299_make_hal(ads1299_platform_hal_t *hal)
{
    int rc;

    if (hal == 0) {
        return -1;
    }

    if (g_ads1299_context.lpspi == 0 ||
        g_ads1299_context.source_clock_hz == 0u ||
        g_ads1299_context.baud_hz == 0u ||
        g_ads1299_context.cs.gpio == 0 ||
        g_ads1299_context.reset.gpio == 0 ||
        g_ads1299_context.pwdn.gpio == 0 ||
        g_ads1299_context.start.gpio == 0 ||
        g_ads1299_context.drdy.gpio == 0 ||
        g_ads1299_context.delay_us == 0) {
        return -2;
    }

    rc = ads1299_mimxrt1042_hal_init(&g_ads1299_context);
    if (rc != 0) {
        return -3;
    }

    *hal = ads1299_mimxrt1042_make_hal(&g_ads1299_context);
    if (hal->spi_transfer == 0 || hal->pin_write == 0 ||
        hal->pin_read == 0 || hal->delay_us == 0) {
        return -4;
    }

    return 0;
}
