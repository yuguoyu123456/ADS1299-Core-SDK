/*
 * LPC54628 ADS1299 board binding.
 *
 * This is the only application-facing bridge between board_ads1299_config.h
 * and the existing MCUXpresso port.  Keep board-specific SPI/GPIO/clock/timer
 * choices in board_ads1299_config.h; do not edit the shared ADS1299 core.
 *
 * Validation status: TEMPLATE.  The default configuration is deliberately
 * invalid and this function fails closed until the board macros are bound to
 * a real MCUXpresso project.
 */

#include "board_ads1299_config.h"

int board_ads1299_get_hw(ads1299_lpc54628_hw_t *out_hw)
{
    if (out_hw == 0) {
        return -1;
    }

    if (!board_ads1299_config_valid()) {
        return -2;
    }

    *out_hw = board_ads1299_hw();
    return 0;
}
