#ifndef BOARD_ADS1299_BINDING_H
#define BOARD_ADS1299_BINDING_H

#include "../ads1299_port/ads1299_spi.h"

/*
 * Build the platform HAL from the single board_ads1299_config.h configuration
 * point.  This is the preferred path for new applications; the older
 * board_ads1299_hal() seam remains available for compatibility.
 *
 * Returns:
 *   0  success
 *  -1  null output pointer
 *  -2  board configuration has not been reviewed/completed
 *  -3  MCUXpresso vendor HAL initialization failed
 *  -4  generated HAL is incomplete
 */
int board_ads1299_make_hal(ads1299_platform_hal_t *hal);

#endif /* BOARD_ADS1299_BINDING_H */
