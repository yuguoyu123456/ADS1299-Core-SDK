#ifndef ADS1299_PSOC6_HAL_H
#define ADS1299_PSOC6_HAL_H
#include "ads1299_spi.h"
#include "cy_scb_spi.h"
#include "cy_gpio.h"
/* Board owns clocks, HSIOM, direction, disabled SCB IRQ/DMA and CM0+ startup.
 * Dedicated SCB on CM4 only. Context must be zero initialized and persistent. */
typedef struct {
    CySCB_Type *spi;
    GPIO_PRT_Type *gpio[5];
    uint32_t pins[5];
    uint32_t poll_limit; /* per wait, iteration count, not microseconds */
    void *delay_user;
    void (*delay_us)(void *,uint32_t);
    int fault;
} ads1299_psoc6_context_t;
/* The caller supplies a real clock-matched Mode 1, 8-bit MSB config.
 * Peripheral SS must not be routed to ADS1299 CS; use GPIO for physical CS. */
int ads1299_psoc6_make_hal(ads1299_psoc6_context_t *ctx,
                         const cy_stc_scb_spi_config_t *config,
                         ads1299_platform_hal_t *hal);
#endif
