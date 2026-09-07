#ifndef ADS1299_PORT_WCH_H
#define ADS1299_PORT_WCH_H

#include "ads1299_port.h"
#include "ch32v30x.h"

#include <stdint.h>

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
} ads1299_wch_ctx_t;

ads1299_port_t ads1299_wch_make_port(ads1299_wch_ctx_t *ctx);

#endif
