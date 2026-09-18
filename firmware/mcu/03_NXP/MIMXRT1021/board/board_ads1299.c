#include "board_ads1299.h"
#include "../mcux_adapter/ads1299_mimxrt1021_hal.h"

/*
 * Reference-board integration seam for MIMXRT1020-EVK / MIMXRT1021.
 *
 * Keep this file board-specific. The MCUXpresso application must provide the
 * concrete board context after its generated clock/pin initialization has run.
 * This deliberately avoids inventing SDK-generated pin symbols in the SDK.
 */
extern ads1299_mimxrt1021_hal_context_t *board_ads1299_context(void);

int board_ads1299_hal(ads1299_platform_hal_t *hal)
{
    ads1299_mimxrt1021_hal_context_t *ctx;
    int rc;

    if (hal == 0) {
        return -1;
    }

    ctx = board_ads1299_context();
    if (ctx == 0) {
        return -2;
    }

    rc = ads1299_mimxrt1021_hal_init(ctx);
    if (rc != 0) {
        return rc;
    }

    *hal = ads1299_mimxrt1021_make_hal(ctx);
    if (hal->spi_transfer == 0 || hal->pin_write == 0 ||
        hal->pin_read == 0 || hal->delay_us == 0) {
        return -5;
    }

    return 0;
}
