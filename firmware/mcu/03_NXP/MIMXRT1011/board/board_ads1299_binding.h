#ifndef BOARD_ADS1299_BINDING_H
#define BOARD_ADS1299_BINDING_H

#include "../ads1299_port/ads1299_spi.h"

/*
 * Build the platform HAL from the single board configuration point.
 *
 * Returns:
 *   0  success
 *  -1  output pointer is NULL
 *  -2  board_ads1299_config.h is incomplete or not reviewed
 *  -3  MCUXpresso/LPSPI HAL initialization failed
 *
 * The backing context has static lifetime so callbacks stored in the returned
 * HAL remain valid after this function returns.
 */
int board_ads1299_make_hal(ads1299_platform_hal_t *out_hal);

#endif /* BOARD_ADS1299_BINDING_H */
