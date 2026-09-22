#ifndef MCXA153_BOARD_ADS1299_BINDING_H
#define MCXA153_BOARD_ADS1299_BINDING_H

#include "ads1299_mcxa153_hal.h"

/*
 * Application-facing board binding.
 *
 * Hardware-dependent values remain in board_ads1299_config.h. Applications
 * and examples should use this function instead of rebuilding the MCXA153
 * context or editing shared ADS1299 core files.
 *
 * Returns:
 *   0  board configuration is complete and HAL was created
 *  -1  hal is NULL
 *  -2  board configuration is incomplete (fail-closed defaults still present)
 *  -3  MCXA153 HAL construction rejected the configured context
 */
int board_ads1299_make_hal(ads1299_platform_hal_t *hal);

#endif /* MCXA153_BOARD_ADS1299_BINDING_H */
