#ifndef ADS1299_TM4C_HAL_H
#define ADS1299_TM4C_HAL_H
#include "ads1299_spi.h"
#include <stdbool.h>
typedef struct {
    uint32_t ssi_base, gpio_base[5];
    uint8_t pins[5]; /* One-bit masks, CS/RESET/PWDN/START/DRDY order. */
    uint32_t source_hz, spi_hz, poll_limit;
    void *delay_user;
    void (*delay_us)(void *,uint32_t);
    int fault;
} ads1299_tm4c_context_t;
/* Caller owns clocks/mux/directions; SSI must be disabled and exclusively owned. */
int ads1299_tm4c_make_hal(ads1299_tm4c_context_t *,ads1299_platform_hal_t *);
#endif
