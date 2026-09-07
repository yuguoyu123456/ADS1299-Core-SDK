#ifndef ADS1299_G4_HAL_H
#define ADS1299_G4_HAL_H
#include "ads1299_spi.h"
#include "stm32g4xx.h"
/* Board owns clock, alternate functions, direction, and exclusive SPI access. */
typedef struct {
    SPI_TypeDef *spi;
    GPIO_TypeDef *gpio[5];
    uint16_t pins[5]; /* one-hot masks */
    uint32_t source_hz, spi_hz, poll_limit;
    void *delay_user;
    void (*delay_us)(void *,uint32_t);
    int fault;
} ads1299_g4_context_t;
int ads1299_g4_make_hal(ads1299_g4_context_t *ctx,ads1299_platform_hal_t *hal);
#endif
