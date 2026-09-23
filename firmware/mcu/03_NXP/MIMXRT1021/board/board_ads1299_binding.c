#include "board_ads1299_binding.h"
#include "board_ads1299_config.h"
#include "../mcux_adapter/ads1299_mimxrt1021_hal.h"

int board_ads1299_make_hal(ads1299_platform_hal_t *hal)
{
    static ads1299_mimxrt1021_hal_context_t context;
    int rc;

    if (hal == 0) {
        return -1;
    }

    if (!board_ads1299_config_valid()) {
        return -2;
    }

    /* Static storage is intentional: platform callbacks retain this context. */
    context = board_ads1299_context();

    rc = ads1299_mimxrt1021_hal_init(&context);
    if (rc != 0) {
        return -3;
    }

    *hal = ads1299_mimxrt1021_make_hal(&context);
    if (hal->spi_transfer == 0 || hal->pin_write == 0 ||
        hal->pin_read == 0 || hal->delay_us == 0) {
        return -4;
    }

    return 0;
}
