/*
 * Compile-only contract check for the MIMXRT1170 ADS1299 board binding.
 *
 * This translation unit deliberately checks only the platform boundary.  It
 * contains no ADS1299 register policy and does not claim that the reference
 * EVK signal mapping has been verified on physical hardware.
 */
#include "ads1299_mcuxpresso_init.h"

#include <stdint.h>

/* Every runnable example needs this single board-owned entry point.  Taking
 * its address turns a missing declaration/signature into a compile failure;
 * linking this check with the selected board source catches a missing
 * implementation as well. */
typedef int (*ads1299_rt1170_board_hal_fn_t)(ads1299_platform_hal_t *hal);

extern int board_ads1299_hal(ads1299_platform_hal_t *hal);

static ads1299_rt1170_board_hal_fn_t const ads1299_rt1170_board_hal_contract =
    board_ads1299_hal;

uintptr_t ads1299_rt1170_board_binding_contract_word(void)
{
    return (uintptr_t)ads1299_rt1170_board_hal_contract;
}
