#include "board_ads1299_binding.h"
#include "board_ads1299_config.h"

int board_ads1299_make_hal(ads1299_platform_hal_t *hal)
{
    ads1299_mcxa153_context_t ctx;
    int rc;

    if (hal == NULL) {
        return -1;
    }
    if (!board_ads1299_config_valid()) {
        return -2;
    }

    ctx = board_ads1299_context();
    rc = ads1299_mcxa153_make_hal(&ctx, hal);
    if (rc != 0) {
        return -3;
    }

    return 0;
}
