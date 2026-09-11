#include <assert.h>

#include "ads1299_spi.h"
#include "stm32h743_beginner_demo.h"

int board_ads1299_hal(ads1299_platform_hal_t *hal)
{
    (void)hal;
    return -1;
}

void test_beginner_demo_build_and_board_failure(void)
{
    /*
     * Host smoke test purpose:
     * 1. compile/link the progressive beginner example against the real shared
     *    ADS1299 and canonical packet APIs;
     * 2. verify a missing board binding fails before touching the shared core.
     *
     * Hardware behavior is intentionally not implied by this host test.
     */
    assert(stm32h743_ads1299_beginner_demo(NULL, 0u) == -10);
}
