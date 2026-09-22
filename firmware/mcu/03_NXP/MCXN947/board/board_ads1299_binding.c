#include "board_ads1299_binding.h"
#include "board_ads1299_config.h"

int board_ads1299_make_hal(ads1299_platform_hal_t *hal)
{
    if (hal == 0) {
        return -1;
    }
    if (!board_ads1299_config_valid()) {
        return -2;
    }

    /* HAL callbacks retain the context pointer, so give it static lifetime. */
    {
        static ads1299_mcxn947_context_t board_ctx;
        board_ctx = board_ads1299_context();
        return ads1299_mcxn947_make_hal(&board_ctx, hal);
    }
}
