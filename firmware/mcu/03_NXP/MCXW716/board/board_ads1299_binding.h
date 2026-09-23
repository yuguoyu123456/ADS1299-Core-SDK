#ifndef BOARD_ADS1299_BINDING_H
#define BOARD_ADS1299_BINDING_H

#include "../ads1299_port/ads1299_spi.h"

/*
 * Build the MCXW716 ADS1299 platform HAL from board_ads1299_config.h.
 *
 * Returns:
 *   0  configuration accepted and HAL returned
 *  -1  out_hal is NULL
 *  -2  board configuration is incomplete or has not been reviewed
 *  -3  MCXW716 vendor HAL initialization failed
 *
 * The context backing the returned callbacks has static storage duration.
 */
int board_ads1299_make_hal(ads1299_platform_hal_t *out_hal);

#endif /* BOARD_ADS1299_BINDING_H */
