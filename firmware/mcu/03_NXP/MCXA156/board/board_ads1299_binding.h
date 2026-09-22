#ifndef BOARD_ADS1299_BINDING_H
#define BOARD_ADS1299_BINDING_H

#include "../mcux_adapter/ads1299_mcxa156_hal.h"

/*
 * Application-facing MCXA156 board binding.
 *
 * Configure hardware only in board_ads1299_config.h, then call this function
 * to obtain the existing platform HAL.  The binding fails closed while the
 * board configuration is incomplete or wiring review is not acknowledged.
 *
 * Returns:
 *   0   HAL constructed successfully
 *  -1   invalid output pointer
 *  -2   board configuration incomplete / wiring not reviewed
 *  other value propagated from ads1299_mcxa156_make_hal()
 */
int board_ads1299_make_hal(ads1299_platform_hal_t *hal);

#endif
