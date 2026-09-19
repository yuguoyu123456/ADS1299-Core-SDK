/* MIMXRT1052 progressive bring-up: ADS1299 probe / ID validation.
 * Board-specific pins and MCUXpresso SDK calls stay behind board_ads1299_hal().
 *
 * Exit codes intentionally separate board/port, initialization/reset, and
 * SPI/identity failures so this can also serve as a simple bring-up smoke test.
 */
#include "ads1299.h"
#include "ads1299_spi.h"

extern int board_ads1299_hal(ads1299_platform_hal_t *hal);

int main(void)
{
    ads1299_platform_hal_t hal;
    ads1299_mcu_port_t mcu;
    ads1299_t dev;
    ads1299_device_id_t id;

    if (board_ads1299_hal(&hal) != 0) return 10;
    if (ads1299_mcu_port_init(&mcu, &hal) != 0) return 11;

    ads1299_port_t port = ads1299_mcu_make_port(&mcu);
    if (ads1299_init(&dev, &port) != ADS1299_OK) return 12;

    if (ads1299_hardware_reset(&dev) != ADS1299_OK) return 20;
    if (ads1299_sdatac(&dev) != ADS1299_OK) return 21;

    if (ads1299_read_device_id(&dev, &id) != ADS1299_OK) return 30;
    if (!id.is_ads1299_family) return 31;

    /* Success: inspect the decoded identity in a debugger or add the board's
     * normal console transport here.  No ADS1299 register literals belong in
     * this platform example. */
    return 0;
}
