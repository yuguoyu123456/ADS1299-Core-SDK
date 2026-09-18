/*
 * Host-side smoke test for the MIMXRT1042 board integration seam.
 *
 * This test intentionally avoids MCUXpresso headers. It verifies that the
 * board contract rejects invalid inputs and refuses an incomplete platform
 * HAL rather than allowing a beginner example to continue with NULL callbacks.
 * The real evkmimxrt1040 target remains TEMPLATE / NOT BUILD-VERIFIED /
 * NOT BOARD-VERIFIED until compiled with the documented MCUXpresso SDK.
 */
#include <assert.h>
#include <string.h>

#include "../board/board_ads1299.h"
#include "../mcux_adapter/ads1299_mimxrt1042_hal.h"

static ads1299_mimxrt1042_hal_context_t g_context;

ads1299_mimxrt1042_hal_context_t *board_ads1299_context(void)
{
    return &g_context;
}

int main(void)
{
    ads1299_platform_hal_t hal;

    assert(board_ads1299_hal(0) == -1);

    /* A zeroed context must never produce a usable HAL. */
    memset(&g_context, 0, sizeof(g_context));
    memset(&hal, 0xA5, sizeof(hal));
    assert(board_ads1299_hal(&hal) != 0);

    return 0;
}
