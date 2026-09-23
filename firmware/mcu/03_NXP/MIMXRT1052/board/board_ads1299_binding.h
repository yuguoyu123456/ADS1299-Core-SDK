#ifndef BOARD_ADS1299_BINDING_H
#define BOARD_ADS1299_BINDING_H

#include "../ads1299_port/ads1299_spi.h"

/*
 * Build the platform HAL from the single board_ads1299_config.h contract.
 * Returns 0 on success; negative values distinguish board/config/HAL failures.
 */
int board_ads1299_make_hal(ads1299_platform_hal_t *hal);

#endif /* BOARD_ADS1299_BINDING_H */
