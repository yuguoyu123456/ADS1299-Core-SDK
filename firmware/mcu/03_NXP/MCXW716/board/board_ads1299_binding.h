#ifndef BOARD_ADS1299_BINDING_H
#define BOARD_ADS1299_BINDING_H

#include "../ads1299_port/ads1299_hal.h"

/*
 * Build the shared ADS1299 HAL from the single MCXW716 board configuration.
 *
 * Returns:
 *   0  success
 *  -1  out_hal is NULL
 *  -2  board_ads1299_config_valid() rejected the board configuration
 *  -3  MCXW716 vendor HAL construction failed
 */
int board_ads1299_make_hal(ads1299_hal_t *out_hal);

#endif /* BOARD_ADS1299_BINDING_H */
