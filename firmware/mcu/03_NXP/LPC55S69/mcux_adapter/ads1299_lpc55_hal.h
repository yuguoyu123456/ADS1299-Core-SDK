#ifndef ADS1299_LPC55_HAL_H
#define ADS1299_LPC55_HAL_H
#include "ads1299_spi.h"
#include "fsl_spi.h"
#include "fsl_gpio.h"
#if SPI_RETRY_TIMES == 0
#error "Compile adapter AND vendor fsl_spi.c with finite SPI_RETRY_TIMES"
#endif
/* Core0 only; board owns clock/mux/GPIO directions and exclusive Flexcomm. */
typedef struct {
    SPI_Type *spi;
    GPIO_Type *gpio;
    uint8_t ports[5],pins[5];
    uint32_t source_hz,spi_hz,poll_limit;
    void *delay_user;
    void (*delay_us)(void *,uint32_t);
    int fault;
} ads1299_lpc55_context_t;
int ads1299_lpc55_make_hal(ads1299_lpc55_context_t *ctx,ads1299_platform_hal_t *hal);
#endif
