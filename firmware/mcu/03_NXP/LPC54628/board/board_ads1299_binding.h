#ifndef LPC54628_BOARD_ADS1299_BINDING_H
#define LPC54628_BOARD_ADS1299_BINDING_H

#include "board_ads1299_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Application-facing board binding.
 *
 * Returns:
 *   0  board configuration is valid and *out_hw is populated
 *  -1  out_hw is NULL
 *  -2  board_ads1299_config.h still contains an invalid/incomplete binding
 *
 * A beginner application should call this function rather than constructing
 * ads1299_lpc54628_hw_t itself. Hardware-dependent values stay in
 * board_ads1299_config.h; shared ADS1299 core files must not be edited.
 */
int board_ads1299_get_hw(ads1299_lpc54628_hw_t *out_hw);

#ifdef __cplusplus
}
#endif

#endif /* LPC54628_BOARD_ADS1299_BINDING_H */
