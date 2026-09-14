#include <limits.h>
#include <stdio.h>
#include <string.h>

#include "ads1299.h"
#include "ads1299_linux_port.h"
#include "board_config.h"

static int config_ready(void) {
    if (RK3399_ADS1299_CS_LINE == UINT_MAX) {
        fprintf(stderr, "CONFIG: set RK3399_ADS1299_CS_LINE in board_config.h before running.\n");
        return 0;
    }
    return 1;
}

int main(void) {
    if (!config_ready()) return 2;

    const rk3399_ads1299_linux_config_t config = {
        .spidev_path = RK3399_ADS1299_SPIDEV,
        .gpiochip_path = RK3399_ADS1299_GPIOCHIP,
        .spi_hz = RK3399_ADS1299_SPI_HZ,
        .cs_line = RK3399_ADS1299_CS_LINE,
        .reset_line = RK3399_ADS1299_RESET_LINE,
        .pwdn_line = RK3399_ADS1299_PWDN_LINE,
        .start_line = RK3399_ADS1299_START_LINE,
        .drdy_line = RK3399_ADS1299_DRDY_LINE,
    };

    rk3399_ads1299_linux_t linux_ctx;
    ads1299_port_t port;
    ads1299_t dev;
    ads1299_device_id_t id;

    int rc = rk3399_ads1299_linux_open(&linux_ctx, &config);
    if (rc != 0) {
        fprintf(stderr, "PORT: open failed (%d: %s). Check spidev/gpiochip permissions, device-tree exposure and line offsets.\n",
                rc, strerror(-rc));
        return 3;
    }

    rc = rk3399_ads1299_linux_bind(&linux_ctx, &port);
    if (rc != 0) {
        fprintf(stderr, "PORT: bind failed (%d).\n", rc);
        rk3399_ads1299_linux_close(&linux_ctx);
        return 4;
    }

    ads1299_status_t status = ads1299_init(&dev, &port);
    if (status != ADS1299_OK) {
        fprintf(stderr, "CORE: ads1299_init failed (%d).\n", (int)status);
        rk3399_ads1299_linux_close(&linux_ctx);
        return 5;
    }

    status = ads1299_read_device_id(&dev, &id);
    if (status != ADS1299_OK) {
        fprintf(stderr, "SPI/ID: probe failed (%d). Check wiring, CS and SPI device selection.\n", (int)status);
        rk3399_ads1299_linux_close(&linux_ctx);
        return 6;
    }

    printf("ADS1299 probe OK: raw_id=0x%02X family=%u channels=%u revision=%u\n",
           id.raw, id.is_ads1299_family, id.channel_count, id.revision);

    if (!id.is_ads1299_family) {
        fprintf(stderr, "ID: response is not recognized as ADS1299 family.\n");
        rk3399_ads1299_linux_close(&linux_ctx);
        return 7;
    }

    rk3399_ads1299_linux_close(&linux_ctx);
    return 0;
}
