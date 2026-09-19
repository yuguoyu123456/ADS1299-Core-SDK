/*
 * MCUXpresso compile-only smoke target for the MIMXRT1052 ADS1299 adapter.
 *
 * Add this translation unit to the same official MCUXpresso project as
 * ads1299_mimxrt1052_hal.c.  It intentionally performs no I/O: its purpose is
 * to make SDK/API drift fail at compile time before hardware bring-up.
 *
 * This is not a hardware test and successful compilation alone must not be
 * reported as BOARD-VERIFIED.
 */
#include "ads1299_mimxrt1052_hal.h"

int ads1299_mimxrt1052_adapter_compile_smoke(
    ads1299_mimxrt1052_hal_context_t *ctx)
{
    ads1299_platform_hal_t hal;

    if (ctx == 0) {
        return -1;
    }

    /* Force the compiler to type-check the real MCUXpresso adapter surface. */
    hal = ads1299_mimxrt1052_make_hal(ctx);

    /* All callbacks required by the portable MCU port must be supplied. */
    if (hal.spi_transfer == 0 ||
        hal.cs_write == 0 ||
        hal.reset_write == 0 ||
        hal.pwdn_write == 0 ||
        hal.start_write == 0 ||
        hal.drdy_read == 0 ||
        hal.delay_us == 0) {
        return -2;
    }

    return 0;
}
