#ifndef ADS1299_RT1166_MCUXPRESSO_HAL_H
#define ADS1299_RT1166_MCUXPRESSO_HAL_H

#include "ads1299_spi.h"
#include "fsl_gpio.h"
#include "fsl_lpspi.h"

typedef struct {
    GPIO_Type *base;
    uint32_t pin;
} ads1299_rt1166_gpio_t;

typedef struct {
    LPSPI_Type *lpspi;
    uint32_t transfer_flags;
    ads1299_rt1166_gpio_t cs;
    ads1299_rt1166_gpio_t reset;
    ads1299_rt1166_gpio_t pwdn;
    ads1299_rt1166_gpio_t start;
    ads1299_rt1166_gpio_t drdy;
} ads1299_rt1166_ctx_t;

/*
 * Bind the portable MIMXRT1166 ADS1299 port to MCUXpresso GPIO/LPSPI.
 * The caller owns ctx for at least as long as the returned HAL is in use.
 * Pin mux, GPIO direction and LPSPI Mode-1/MSB-first configuration remain
 * board-project responsibilities and should be sourced from board config.
 */
int ads1299_rt1166_make_hal(ads1299_platform_hal_t *hal,
                            ads1299_rt1166_ctx_t *ctx);

#endif
