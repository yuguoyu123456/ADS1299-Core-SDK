#ifndef ADS1299_MCUXPRESSO_HAL_H
#define ADS1299_MCUXPRESSO_HAL_H

#include "ads1299_spi.h"
#include "fsl_gpio.h"
#include "fsl_lpspi.h"

typedef struct {
    GPIO_Type *base;
    uint32_t pin;
} ads1299_mcuxpresso_gpio_t;

typedef struct {
    LPSPI_Type *lpspi;
    uint32_t transfer_flags;
    ads1299_mcuxpresso_gpio_t cs;
    ads1299_mcuxpresso_gpio_t reset;
    ads1299_mcuxpresso_gpio_t pwdn;
    ads1299_mcuxpresso_gpio_t start;
    ads1299_mcuxpresso_gpio_t drdy;
} ads1299_mcuxpresso_ctx_t;

int ads1299_mcuxpresso_make_hal(ads1299_platform_hal_t *hal,
                                 ads1299_mcuxpresso_ctx_t *ctx);

#endif
