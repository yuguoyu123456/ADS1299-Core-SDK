#ifndef ADS1299_U5_HAL_H
#define ADS1299_U5_HAL_H
#include "ads1299_spi.h"
#include "stm32u5xx.h"
/* U575 SPI1 reference. Board owns security attribution, clocks, mux and reset. */
typedef struct {
    SPI_TypeDef *spi;
    GPIO_TypeDef *gpio[5];
    uint16_t pins[5];
    uint32_t source_hz,spi_hz,poll_limit;
    uint32_t eot_delay_us; /* initialized by make_hal from actual baud divisor */
    void *delay_user;
    void (*delay_us)(void *,uint32_t);
    int fault;
} ads1299_u5_context_t;
int ads1299_u5_make_hal(ads1299_u5_context_t *ctx,ads1299_platform_hal_t *hal);
#endif
