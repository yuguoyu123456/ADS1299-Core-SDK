#include "board_ads1299_binding.h"
#include "board_ads1299_config.h"
#include "../mcux_adapter/ads1299_mimxrt1011_hal.h"

int board_ads1299_make_hal(ads1299_platform_hal_t *out_hal)
{
    static ads1299_mimxrt1011_hal_context_t context;

    if (out_hal == 0) {
        return -1;
    }

    if (!board_ads1299_config_valid()) {
        return -2;
    }

    context = board_ads1299_context();
    if (ads1299_mimxrt1011_hal_init(&context) != 0) {
        return -3;
    }

    *out_hal = ads1299_mimxrt1011_make_hal(&context);
    return 0;
}
