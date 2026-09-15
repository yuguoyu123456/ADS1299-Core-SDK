#ifndef ADS1299_MIMXRT1042_HAL_H
#define ADS1299_MIMXRT1042_HAL_H

#include <stdint.h>
#include "fsl_gpio.h"
#include "fsl_lpspi.h"
#include "../ads1299_port/ads1299_spi.h"

typedef struct {
    GPIO_Type *gpio;
    uint32_t pin;
} ads1299_mimxrt1042_gpio_t;

typedef struct {
    LPSPI_Type *lpspi;
    uint32_t source_clock_hz;
    uint32_t baud_hz;
    ads1299_mimxrt1042_gpio_t cs;
    ads1299_mimxrt1042_gpio_t reset;
    ads1299_mimxrt1042_gpio_t pwdn;
    ads1299_mimxrt1042_gpio_t start;
    ads1299_mimxrt1042_gpio_t drdy;
    void (*delay_us)(uint32_t us);
} ads1299_mimxrt1042_hal_context_t;

/* Configure LPSPI for ADS1299 Mode 1/MSB-first/8-bit transfers and validate GPIOs. */
int ads1299_mimxrt1042_hal_init(ads1299_mimxrt1042_hal_context_t *ctx);

/* Return the existing platform HAL consumed by ads1299_mcu_port_init(). */
ads1299_platform_hal_t ads1299_mimxrt1042_make_hal(ads1299_mimxrt1042_hal_context_t *ctx);

#endif
