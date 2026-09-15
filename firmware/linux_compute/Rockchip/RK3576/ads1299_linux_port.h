#ifndef RK3576_ADS1299_LINUX_PORT_H
#define RK3576_ADS1299_LINUX_PORT_H

#include <stdint.h>
#include "ads1299_port.h"

typedef struct {
    const char *spidev_path;
    const char *gpiochip_path;
    uint32_t spi_hz;
    unsigned int cs_line;
    unsigned int reset_line;
    unsigned int pwdn_line;
    unsigned int start_line;
    unsigned int drdy_line;
} rk3576_ads1299_linux_config_t;

typedef struct {
    int spi_fd;
    int cs_fd;
    int reset_fd;
    int pwdn_fd;
    int start_fd;
    int drdy_fd;
    uint32_t spi_hz;
} rk3576_ads1299_linux_t;

int rk3576_ads1299_linux_open(rk3576_ads1299_linux_t *ctx,
                              const rk3576_ads1299_linux_config_t *config);
void rk3576_ads1299_linux_close(rk3576_ads1299_linux_t *ctx);
int rk3576_ads1299_linux_bind(rk3576_ads1299_linux_t *ctx,
                              ads1299_port_t *port);

#endif
