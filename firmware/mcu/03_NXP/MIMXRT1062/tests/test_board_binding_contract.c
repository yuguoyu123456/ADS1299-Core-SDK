/*
 * Compile-time smoke test for the MIMXRT1062 board/HAL binding.
 *
 * This deliberately does not touch hardware.  It protects the public board
 * entry points used by beginner examples and keeps their platform HAL type
 * tied to the shared ADS1299 platform contract.
 */
#include "board_ads1299_binding.h"

static int (*const make_hal_fn)(ads1299_platform_hal_t *) = board_ads1299_make_hal;
static int (*const compatibility_hal_fn)(ads1299_platform_hal_t *) = board_ads1299_hal;

int main(void)
{
    /* A null pointer must remain a valid compile-time argument shape.  Do not
     * call either function here: they intentionally configure MCU clocks and
     * pins when executed on the target. */
    return (make_hal_fn != 0 && compatibility_hal_fn != 0) ? 0 : 1;
}
