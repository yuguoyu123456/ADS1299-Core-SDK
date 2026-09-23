#ifndef BOARD_ADS1299_BINDING_H
#define BOARD_ADS1299_BINDING_H

#include "board_ads1299.h"

/*
 * MIMXRT1042 board-to-HAL binding.
 * TEMPLATE / NOT BUILD-VERIFIED / NOT BOARD-VERIFIED.
 *
 * Applications configure hardware only through board_ads1299_config.h (or
 * equivalent BOARD_ADS1299_* build definitions) and provide the declared
 * delay callback. Shared ADS1299 core files remain untouched.
 */
int board_ads1299_make_hal(ads1299_platform_hal_t *hal);

#endif /* BOARD_ADS1299_BINDING_H */
