#ifndef ADS1299_LPC54628_MCUXPRESSO_H
#define ADS1299_LPC54628_MCUXPRESSO_H

#include <stdint.h>
#include "fsl_spi.h"
#include "fsl_gpio.h"
#include "ads1299_spi.h"

typedef struct {
    GPIO_Type *gpio;
    uint32_t port;
    uint32_t pin;
} ads1299_lpc54628_gpio_t;

typedef struct {
    SPI_Type *spi;
    uint32_t spi_src_clock_hz;
    uint32_t baud_hz;
    ads1299_lpc54628_gpio_t cs;
    ads1299_lpc54628_gpio_t reset;
    ads1299_lpc54628_gpio_t pwdn;
    ads1299_lpc54628_gpio_t start;
    ads1299_lpc54628_gpio_t drdy;
    void (*delay_us)(uint32_t us);
} ads1299_lpc54628_hw_t;

/* Bind an already pin-muxed/clocked MCUXpresso board configuration to the
 * portable LPC54628 ADS1299 port. This function configures the SPI master and
 * GPIO directions, but intentionally does not invent board pin mux or clocks. */
int ads1299_lpc54628_mcuxpresso_init(ads1299_lpc54628_hw_t *hw,
                                     ads1299_platform_hal_t *hal);

#endif
