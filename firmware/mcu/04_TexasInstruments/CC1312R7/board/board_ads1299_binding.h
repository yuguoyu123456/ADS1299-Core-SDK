#ifndef CC1312R7_BOARD_ADS1299_BINDING_H
#define CC1312R7_BOARD_ADS1299_BINDING_H

#include "board_ads1299_config.h"
#include "ads1299_spi.h"

/*
 * Board-to-port binding contract.
 *
 * The implementation is deliberately supplied by the selected TI
 * SimpleLink/SysConfig project because generated SPI/GPIO identifiers are
 * project-specific.  This header gives applications one stable entry point
 * without leaking those identifiers into the shared ADS1299 core.
 */
typedef enum {
    CC1312R7_ADS1299_BIND_OK = 0,
    CC1312R7_ADS1299_BIND_ERR_ARGUMENT = -1,
    CC1312R7_ADS1299_BIND_ERR_CONFIG = -2,
    CC1312R7_ADS1299_BIND_ERR_SDK = -3
} cc1312r7_ads1299_bind_result_t;

/*
 * Create a platform HAL from the single board configuration point.
 * A TI SDK adapter should configure SPI as controller/master, Mode 1
 * (CPOL=0, CPHA=1), MSB first, using cfg->spi_hz, then bind the five GPIOs.
 * No ADS1299 register programming belongs in this function.
 */
int cc1312r7_ads1299_make_hal(
    const cc1312r7_ads1299_board_config_t *cfg,
    ads1299_platform_hal_t *out_hal);

#endif
