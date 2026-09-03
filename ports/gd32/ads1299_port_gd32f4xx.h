#ifndef ADS1299_PORT_GD32F4XX_H
#define ADS1299_PORT_GD32F4XX_H

#include "gd32f4xx.h"
#include "ads1299_port.h"

typedef struct {
    uint32_t spi_periph;
    uint32_t cs_port;
    uint32_t cs_pin;
    uint32_t reset_port;
    uint32_t reset_pin;
    uint32_t start_port;
    uint32_t start_pin;
    uint32_t drdy_port;
    uint32_t drdy_pin;
    uint32_t spin_limit;
    void (*delay_us_cb)(uint32_t us);
} ads1299_gd32f4xx_ctx_t;

ads1299_port_t ads1299_gd32f4xx_make_port(ads1299_gd32f4xx_ctx_t *ctx);

#endif
