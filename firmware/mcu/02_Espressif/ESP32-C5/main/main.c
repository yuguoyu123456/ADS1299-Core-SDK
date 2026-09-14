#include "ads1299_espidf_hal.h"

void app_main(void)
{
    ads1299_espidf_hal_ctx_t hal_ctx = {0};
    ads1299_platform_hal_t platform_hal = {0};

    /*
     * Native build smoke entrypoint only. Runtime bring-up remains in the
     * documented examples so CI does not imply attached ADS1299 hardware.
     */
    (void)hal_ctx;
    (void)platform_hal;
}
