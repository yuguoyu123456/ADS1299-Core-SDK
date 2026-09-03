#ifndef ADS1299_PORT_CH32V30X_H
#define ADS1299_PORT_CH32V30X_H

#include "ch32v30x.h"
#include "ads1299_port.h"

typedef struct {
    SPI_TypeDef *spi;
    GPIO_TypeDef *cs_port;
    uint16_t cs_pin;
    GPIO_TypeDef *reset_port;
    uint16_t reset_pin;
    GPIO_TypeDef *start_port;
    uint16_t start_pin;
    GPIO_TypeDef *drdy_port;
    uint16_t drdy_pin;
    uint32_t spin_limit;
    void (*delay_us_cb)(uint32_t us);
} ads1299_ch32v30x_ctx_t;

ads1299_port_t ads1299_ch32v30x_make_port(ads1299_ch32v30x_ctx_t *ctx);

#endif
