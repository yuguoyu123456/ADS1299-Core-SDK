#ifndef ADS1299_STM32CUBE_HAL_H
#define ADS1299_STM32CUBE_HAL_H

#include "ads1299_spi.h"
#include "stm32h7xx_hal.h"

typedef struct {
    GPIO_TypeDef *port;
    uint16_t pin;
} ads1299_stm32cube_gpio_t;

typedef struct {
    SPI_HandleTypeDef *spi;
    ads1299_stm32cube_gpio_t gpio[5];
    uint32_t spi_timeout_ms;
} ads1299_stm32cube_context_t;

int ads1299_stm32cube_make_hal(ads1299_platform_hal_t *hal,
                               ads1299_stm32cube_context_t *context);

#endif
