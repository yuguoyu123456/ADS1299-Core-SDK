#ifndef LPC54628_BOARD_ADS1299_CONFIG_H
#define LPC54628_BOARD_ADS1299_CONFIG_H

/*
 * Single board-dependent configuration point for the LPC54628 ADS1299 port.
 *
 * This header intentionally contains no guessed evaluation-board pin numbers.
 * Copy the values from the MCUXpresso project generated for the exact board,
 * then include this header from the application's board binding source.
 * Shared ADS1299 core files must not be edited for board bring-up.
 */

#include <stdint.h>
#include "fsl_gpio.h"
#include "fsl_spi.h"
#include "ads1299_lpc54628_mcuxpresso.h"

#ifndef BOARD_ADS1299_SPI
#define BOARD_ADS1299_SPI ((SPI_Type *)0)
#endif

#ifndef BOARD_ADS1299_SPI_SRC_CLOCK_HZ
#define BOARD_ADS1299_SPI_SRC_CLOCK_HZ 0u
#endif

/* Keep the actual ADS1299 SPI baud conservative for initial bring-up. */
#ifndef BOARD_ADS1299_SPI_BAUD_HZ
#define BOARD_ADS1299_SPI_BAUD_HZ 1000000u
#endif

/* GPIO controller plus port/pin values must be supplied by the board project. */
#ifndef BOARD_ADS1299_GPIO
#define BOARD_ADS1299_GPIO ((GPIO_Type *)0)
#endif

#ifndef BOARD_ADS1299_CS_PORT
#define BOARD_ADS1299_CS_PORT 0u
#define BOARD_ADS1299_CS_PIN  0u
#endif
#ifndef BOARD_ADS1299_RESET_PORT
#define BOARD_ADS1299_RESET_PORT 0u
#define BOARD_ADS1299_RESET_PIN  0u
#endif
#ifndef BOARD_ADS1299_PWDN_PORT
#define BOARD_ADS1299_PWDN_PORT 0u
#define BOARD_ADS1299_PWDN_PIN  0u
#endif
#ifndef BOARD_ADS1299_START_PORT
#define BOARD_ADS1299_START_PORT 0u
#define BOARD_ADS1299_START_PIN  0u
#endif
#ifndef BOARD_ADS1299_DRDY_PORT
#define BOARD_ADS1299_DRDY_PORT 0u
#define BOARD_ADS1299_DRDY_PIN  0u
#endif

/* The application supplies a microsecond delay backed by its real board timer. */
#ifndef BOARD_ADS1299_DELAY_US
#define BOARD_ADS1299_DELAY_US ((void (*)(uint32_t))0)
#endif

static inline int board_ads1299_config_valid(void)
{
    return (BOARD_ADS1299_SPI != (SPI_Type *)0) &&
           (BOARD_ADS1299_GPIO != (GPIO_Type *)0) &&
           (BOARD_ADS1299_SPI_SRC_CLOCK_HZ != 0u) &&
           (BOARD_ADS1299_SPI_BAUD_HZ != 0u) &&
           (BOARD_ADS1299_DELAY_US != (void (*)(uint32_t))0);
}

static inline ads1299_lpc54628_hw_t board_ads1299_hw(void)
{
    ads1299_lpc54628_hw_t hw;
    hw.spi = BOARD_ADS1299_SPI;
    hw.spi_src_clock_hz = BOARD_ADS1299_SPI_SRC_CLOCK_HZ;
    hw.baud_hz = BOARD_ADS1299_SPI_BAUD_HZ;
    hw.cs = (ads1299_lpc54628_gpio_t){BOARD_ADS1299_GPIO, BOARD_ADS1299_CS_PORT, BOARD_ADS1299_CS_PIN};
    hw.reset = (ads1299_lpc54628_gpio_t){BOARD_ADS1299_GPIO, BOARD_ADS1299_RESET_PORT, BOARD_ADS1299_RESET_PIN};
    hw.pwdn = (ads1299_lpc54628_gpio_t){BOARD_ADS1299_GPIO, BOARD_ADS1299_PWDN_PORT, BOARD_ADS1299_PWDN_PIN};
    hw.start = (ads1299_lpc54628_gpio_t){BOARD_ADS1299_GPIO, BOARD_ADS1299_START_PORT, BOARD_ADS1299_START_PIN};
    hw.drdy = (ads1299_lpc54628_gpio_t){BOARD_ADS1299_GPIO, BOARD_ADS1299_DRDY_PORT, BOARD_ADS1299_DRDY_PIN};
    hw.delay_us = BOARD_ADS1299_DELAY_US;
    return hw;
}

#endif
