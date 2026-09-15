#ifndef ADS1299_MCXA156_HAL_H
#define ADS1299_MCXA156_HAL_H

#include <stddef.h>
#include <stdint.h>
#include "ads1299_spi.h"
#include "fsl_lpspi.h"
#include "fsl_gpio.h"

/* Board code owns clocks, pin mux, GPIO directions, and the selected LPSPI. */
typedef struct {
    LPSPI_Type *spi;
    GPIO_Type *gpio;
    uint8_t ports[5];
    uint8_t pins[5];
    uint32_t source_hz;
    uint32_t spi_hz;
    uint32_t poll_limit;
    void *delay_user;
    void (*delay_us)(void *user, uint32_t us);
    int fault;
} ads1299_mcxa156_context_t;

int ads1299_mcxa156_make_hal(ads1299_mcxa156_context_t *ctx,
                             ads1299_platform_hal_t *hal);

#endif
