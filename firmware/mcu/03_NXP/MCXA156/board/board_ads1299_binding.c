#include "board_ads1299_binding.h"
#include "board_ads1299_config.h"

int board_ads1299_make_hal(ads1299_platform_hal_t *hal)
{
    static ads1299_mcxa156_context_t ctx;

    if (hal == 0) {
        return -1;
    }
    if (!board_ads1299_config_valid()) {
        return -2;
    }

    ctx = board_ads1299_context();
    return ads1299_mcxa156_make_hal(&ctx, hal);
}
