#include "board_ads1299_binding.h"
#include "board_ads1299_config.h"

int board_ads1299_make_hal(ads1299_platform_hal_t *hal)
{
    ads1299_mcxn236_context_t ctx;

    if (hal == 0) {
        return -1;
    }
    if (!board_ads1299_config_valid()) {
        return -2;
    }

    /*
     * The platform HAL callbacks retain this context pointer, so it must have
     * static lifetime. Rebuilding it here also makes repeated bring-up calls
     * deterministic after board configuration changes at compile time.
     */
    {
        static ads1299_mcxn236_context_t board_ctx;
        board_ctx = board_ads1299_context();
        ctx = board_ctx;
        (void)ctx;
        return ads1299_mcxn236_make_hal(&board_ctx, hal);
    }
}
