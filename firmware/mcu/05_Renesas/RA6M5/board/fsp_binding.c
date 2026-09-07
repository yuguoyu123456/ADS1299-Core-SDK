/* Optional binding compiled in the USER'S generated FSP project.
 * ads1299_board_config.h supplies verified control pins and timer callbacks;
 * see fsp_configuration.md. Do not add another application main(). */
#include "hal_data.h"
#include "ads1299_fsp_hal.h"
#include "ads1299_board_config.h"

static ads1299_fsp_context_t context;

int board_ads1299_hal(ads1299_platform_hal_t *hal)
{
    /* IOPORT and the independent timer must already be initialized by board
     * startup. g_spi0 must NOT already be open: the adapter owns its lifecycle. */
    if (context.opened) return -1;
    context.spi = &g_spi0;
    context.io = &g_ioport;
    context.pins[ADS1299_PIN_CS] = ADS1299_BOARD_CS;
    context.pins[ADS1299_PIN_RESET] = ADS1299_BOARD_RESET;
    context.pins[ADS1299_PIN_PWDN] = ADS1299_BOARD_PWDN;
    context.pins[ADS1299_PIN_START] = ADS1299_BOARD_START;
    context.pins[ADS1299_PIN_DRDY] = ADS1299_BOARD_DRDY;
    context.now_us = ads1299_board_now_us;
    context.delay_us = ads1299_board_delay_us;
    context.timeout_us = 10000;
    return ads1299_fsp_open(&context, hal);
}
