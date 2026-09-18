#include "board_ads1299.h"
#include "../mcux_adapter/ads1299_mimxrt1042_hal.h"

/*
 * Reference-board integration seam for MIMXRT1040-EVK / MIMXRT1042.
 *
 * Keep concrete MCUXpresso-generated clock/pin symbols in the application
 * board layer. The application supplies board_ads1299_context() after its
 * normal BOARD_InitBootPins()/BOARD_InitBootClocks() sequence has completed.
 * This keeps beginner examples and the shared ADS1299 core board-independent.
 */
extern ads1299_mimxrt1042_hal_context_t *board_ads1299_context(void);

int board_ads1299_hal(ads1299_platform_hal_t *hal)
{
    ads1299_mimxrt1042_hal_context_t *ctx;
    int rc;

    if (hal == 0) {
        return -1;
    }

    ctx = board_ads1299_context();
    if (ctx == 0) {
        return -2;
    }

    rc = ads1299_mimxrt1042_hal_init(ctx);
    if (rc != 0) {
        return rc;
    }

    *hal = ads1299_mimxrt1042_make_hal(ctx);
    if (hal->spi_transfer == 0 || hal->pin_write == 0 ||
        hal->pin_read == 0 || hal->delay_us == 0) {
        return -5;
    }

    return 0;
}
