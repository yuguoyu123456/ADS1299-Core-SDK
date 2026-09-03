#ifndef ADS1299_PORT_STM32_HAL_H
#define ADS1299_PORT_STM32_HAL_H

#include "main.h"
#include "ads1299_port.h"

typedef struct {
    SPI_HandleTypeDef *hspi;
    GPIO_TypeDef *cs_port;
    uint16_t cs_pin;
    GPIO_TypeDef *reset_port;
    uint16_t reset_pin;
    GPIO_TypeDef *start_port;
    uint16_t start_pin;
    GPIO_TypeDef *drdy_port;
    uint16_t drdy_pin;
    uint32_t spi_timeout_ms;
    void (*delay_us_cb)(uint32_t us);
} ads1299_stm32_hal_ctx_t;

ads1299_port_t ads1299_stm32_hal_make_port(ads1299_stm32_hal_ctx_t *ctx);

#endif
