#ifndef BOARD_ADS1299_BINDING_H
#define BOARD_ADS1299_BINDING_H

#include "../mcux_adapter/ads1299_mcxn947_hal.h"

/*
 * Beginner-facing bridge from board_ads1299_config.h to the MCXN947 HAL.
 *
 * Returns:
 *   0  HAL created successfully
 *  -1  hal is NULL
 *  -2  board configuration is incomplete / not reviewed
 *  otherwise the platform adapter error from ads1299_mcxn947_make_hal().
 *
 * Board clocks, pin mux and GPIO directions still belong to the surrounding
 * MCUXpresso board project. Do not edit the shared ADS1299 core for bring-up.
 */
int board_ads1299_make_hal(ads1299_platform_hal_t *hal);

#endif /* BOARD_ADS1299_BINDING_H */
