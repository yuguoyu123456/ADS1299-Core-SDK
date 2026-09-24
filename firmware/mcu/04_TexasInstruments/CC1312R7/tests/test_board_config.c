#include <assert.h>

#include "../board/board_ads1299_config.h"

int main(void)
{
    cc1312r7_ads1299_board_config_t cfg =
        cc1312r7_ads1299_board_config_default();

    assert(cfg.spi_hz == CC1312R7_ADS1299_DEFAULT_SPI_HZ);
    assert(!cc1312r7_ads1299_board_config_is_complete(&cfg));

    cfg.spi_instance = 0;
    cfg.pin_cs = 1;
    cfg.pin_drdy = 2;
    cfg.pin_reset = 3;
    cfg.pin_pwdn = 4;
    cfg.pin_start = 5;
    assert(cc1312r7_ads1299_board_config_is_complete(&cfg));

    cfg.spi_hz = 0u;
    assert(!cc1312r7_ads1299_board_config_is_complete(&cfg));

    cfg.spi_hz = 4000001u;
    assert(!cc1312r7_ads1299_board_config_is_complete(&cfg));

    cfg.spi_hz = 4000000u;
    assert(cc1312r7_ads1299_board_config_is_complete(&cfg));

    return 0;
}
