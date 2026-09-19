/*
 * Compile-only contract check for the MIMXRT1062 MCUXpresso adapter.
 *
 * Add this translation unit to the same MCUXpresso project as
 * ads1299_mcuxpresso_hal.c.  It intentionally performs no hardware I/O;
 * successful compilation catches API/type drift between the portable MCU
 * port and the NXP adapter.  Hardware validation is a separate step.
 */
#include <stddef.h>

#include "ads1299_mcuxpresso_hal.h"

int ads1299_rt1062_mcuxpresso_compile_smoke(void)
{
    ads1299_platform_hal_t hal;
    ads1299_rt1062_ctx_t ctx = {0};

    /* These assignments deliberately exercise every adapter context field. */
    ctx.lpspi = (LPSPI_Type *)0;
    ctx.transfer_flags = 0u;

    ctx.cs.base = (GPIO_Type *)0;
    ctx.cs.pin = 0u;
    ctx.reset.base = (GPIO_Type *)0;
    ctx.reset.pin = 0u;
    ctx.pwdn.base = (GPIO_Type *)0;
    ctx.pwdn.pin = 0u;
    ctx.start.base = (GPIO_Type *)0;
    ctx.start.pin = 0u;
    ctx.drdy.base = (GPIO_Type *)0;
    ctx.drdy.pin = 0u;

    return ads1299_rt1062_make_hal(&hal, &ctx);
}
