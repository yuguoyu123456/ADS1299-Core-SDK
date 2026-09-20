/* MIMXRT1189 beginner step 1: transport bring-up and ADS1299 ID probe.
 * Board-dependent LPSPI, clocks and GPIO remain in the model board layer.
 * ADS1299 register semantics remain in the shared core. */
#include "ads1299.h"
#include "ads1299_spi.h"

extern int board_ads1299_hal(ads1299_platform_hal_t *hal);

/* Debugger-visible diagnostics: 0 = success; nonzero = failing stage below. */
volatile ads1299_device_id_t ads1299_probe_identity;
volatile int ads1299_probe_result;

int main(void) {
    ads1299_platform_hal_t hal;
    ads1299_mcu_port_t mcu;
    ads1299_t dev;

    ads1299_probe_result = 1;
    if (board_ads1299_hal(&hal) != 0) return 1;

    ads1299_probe_result = 2;
    if (ads1299_mcu_port_init(&mcu, &hal) != 0) return 2;

    ads1299_port_t port = ads1299_mcu_make_port(&mcu);
    ads1299_probe_result = 3;
    if (ads1299_init(&dev, &port) != ADS1299_OK) return 3;

    ads1299_probe_result = 4;
    if (ads1299_hardware_reset(&dev) != ADS1299_OK) return 4;

    ads1299_probe_result = 5;
    if (ads1299_sdatac(&dev) != ADS1299_OK) return 5;

    ads1299_probe_result = 6;
    if (ads1299_read_device_id(&dev,
            (ads1299_device_id_t *)&ads1299_probe_identity) != ADS1299_OK)
        return 6;

    if (!ads1299_probe_identity.is_ads1299_family) {
        ads1299_probe_result = 7;
        return 7;
    }

    ads1299_probe_result = 0;
    for (;;) { }
}
