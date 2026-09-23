#ifndef BOARD_ADS1299_BINDING_H
#define BOARD_ADS1299_BINDING_H

#include "ads1299_mcuxpresso_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Build the ADS1299 platform HAL from the single board configuration.
 * Return values distinguish integration failures before any ADS1299 profile runs:
 *   0  success
 *  -1  null output pointer
 *  -2  board/peripheral clock configuration invalid
 *  -3  MCUXpresso HAL initialization failed
 *
 * This layer owns only MCU clocks, pin mux and GPIO/SPI binding. ADS1299 register
 * programming remains in the shared core.
 */
int board_ads1299_make_hal(ads1299_platform_hal_t *hal);

/* Compatibility entry point retained for existing examples/applications. */
int board_ads1299_hal(ads1299_platform_hal_t *hal);

#ifdef __cplusplus
}
#endif

#endif
