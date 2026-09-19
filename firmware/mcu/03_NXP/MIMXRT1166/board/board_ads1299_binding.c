/*
 * Generic MIMXRT1166 board/config -> MCUXpresso ADS1299 HAL binding.
 *
 * The consuming MCUXpresso board project owns pin mux, GPIO directions and
 * LPSPI clock/peripheral setup.  Define the ADS1299_BOARD_* values in that
 * project's board configuration before this translation unit includes
 * board_ads1299_config.h.  No reference-board pinout is invented here.
 */
#include "board_ads1299_config.h"
#include "ads1299_mcuxpresso_hal.h"

int board_ads1299_make_hal(ads1299_platform_hal_t *hal)
{
    static ads1299_rt1166_ctx_t ctx = {
        .lpspi = ADS1299_BOARD_LPSPI_BASE,
        .cs = { ADS1299_BOARD_CS_GPIO, ADS1299_BOARD_CS_PIN },
        .reset = { ADS1299_BOARD_RESET_GPIO, ADS1299_BOARD_RESET_PIN },
        .pwdn = { ADS1299_BOARD_PWDN_GPIO, ADS1299_BOARD_PWDN_PIN },
        .start = { ADS1299_BOARD_START_GPIO, ADS1299_BOARD_START_PIN },
        .drdy = { ADS1299_BOARD_DRDY_GPIO, ADS1299_BOARD_DRDY_PIN },
    };

    if (hal == 0) {
        return -1;
    }

    return ads1299_rt1166_make_hal(hal, &ctx);
}
