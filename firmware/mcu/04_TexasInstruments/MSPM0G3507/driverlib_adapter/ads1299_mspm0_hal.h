#ifndef ADS1299_MSPM0_HAL_H
#define ADS1299_MSPM0_HAL_H

#include "ads1299_spi.h"
#include <ti/driverlib/dl_gpio.h>
#include <ti/driverlib/dl_spi.h>

typedef struct { GPIO_Regs *port; uint32_t mask; } ads1299_mspm0_gpio_t;
typedef struct {
    SPI_Regs *spi;
    uint32_t mclk_hz;
    ads1299_mspm0_gpio_t cs, reset, pwdn, start, drdy;
} ads1299_mspm0_ctx_t;

int ads1299_mspm0_make_hal(ads1299_platform_hal_t *hal,
                            ads1299_mspm0_ctx_t *ctx);
#endif
