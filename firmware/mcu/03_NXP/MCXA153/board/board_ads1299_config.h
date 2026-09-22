#ifndef MCXA153_BOARD_ADS1299_CONFIG_H
#define MCXA153_BOARD_ADS1299_CONFIG_H

#include <stdint.h>
#include "ads1299_mcxa153_hal.h"

/*
 * Single board-dependent configuration point for MCXA153 ADS1299 bring-up.
 *
 * This leaf is intentionally fail-closed: the repository does not guess GPIO
 * pins, an LPSPI instance, its source clock, or a microsecond timer for a board
 * whose routing has not been reviewed. Fill these values from the exact board
 * schematic and MCUXpresso project. Shared ADS1299 core files must not change.
 */
#ifndef BOARD_ADS1299_SPI
#define BOARD_ADS1299_SPI ((LPSPI_Type *)0)
#endif

#ifndef BOARD_ADS1299_GPIO
#define BOARD_ADS1299_GPIO ((GPIO_Type *)0)
#endif

#ifndef BOARD_ADS1299_SPI_SOURCE_HZ
#define BOARD_ADS1299_SPI_SOURCE_HZ 0u
#endif

#ifndef BOARD_ADS1299_SPI_HZ
#define BOARD_ADS1299_SPI_HZ 1000000u
#endif

#ifndef BOARD_ADS1299_POLL_LIMIT
#define BOARD_ADS1299_POLL_LIMIT 100000u
#endif

/* GPIO slots follow the existing MCXA153 HAL contract: CS, RESET, PWDN, START, DRDY. */
#ifndef BOARD_ADS1299_CS_PORT
#define BOARD_ADS1299_CS_PORT 0xFFu
#endif
#ifndef BOARD_ADS1299_CS_PIN
#define BOARD_ADS1299_CS_PIN 0xFFu
#endif
#ifndef BOARD_ADS1299_RESET_PORT
#define BOARD_ADS1299_RESET_PORT 0xFFu
#endif
#ifndef BOARD_ADS1299_RESET_PIN
#define BOARD_ADS1299_RESET_PIN 0xFFu
#endif
#ifndef BOARD_ADS1299_PWDN_PORT
#define BOARD_ADS1299_PWDN_PORT 0xFFu
#endif
#ifndef BOARD_ADS1299_PWDN_PIN
#define BOARD_ADS1299_PWDN_PIN 0xFFu
#endif
#ifndef BOARD_ADS1299_START_PORT
#define BOARD_ADS1299_START_PORT 0xFFu
#endif
#ifndef BOARD_ADS1299_START_PIN
#define BOARD_ADS1299_START_PIN 0xFFu
#endif
#ifndef BOARD_ADS1299_DRDY_PORT
#define BOARD_ADS1299_DRDY_PORT 0xFFu
#endif
#ifndef BOARD_ADS1299_DRDY_PIN
#define BOARD_ADS1299_DRDY_PIN 0xFFu
#endif

/* Supply a real microsecond delay from the consuming MCUXpresso project. */
#ifndef BOARD_ADS1299_DELAY_USER
#define BOARD_ADS1299_DELAY_USER ((void *)0)
#endif
#ifndef BOARD_ADS1299_DELAY_US
#define BOARD_ADS1299_DELAY_US ((void (*)(void *, uint32_t))0)
#endif

static inline int board_ads1299_config_valid(void)
{
    return (BOARD_ADS1299_SPI != (LPSPI_Type *)0) &&
           (BOARD_ADS1299_GPIO != (GPIO_Type *)0) &&
           (BOARD_ADS1299_SPI_SOURCE_HZ != 0u) &&
           (BOARD_ADS1299_SPI_HZ != 0u) &&
           (BOARD_ADS1299_CS_PORT != 0xFFu) && (BOARD_ADS1299_CS_PIN != 0xFFu) &&
           (BOARD_ADS1299_RESET_PORT != 0xFFu) && (BOARD_ADS1299_RESET_PIN != 0xFFu) &&
           (BOARD_ADS1299_PWDN_PORT != 0xFFu) && (BOARD_ADS1299_PWDN_PIN != 0xFFu) &&
           (BOARD_ADS1299_START_PORT != 0xFFu) && (BOARD_ADS1299_START_PIN != 0xFFu) &&
           (BOARD_ADS1299_DRDY_PORT != 0xFFu) && (BOARD_ADS1299_DRDY_PIN != 0xFFu) &&
           (BOARD_ADS1299_DELAY_US != (void (*)(void *, uint32_t))0);
}

static inline ads1299_mcxa153_context_t board_ads1299_context(void)
{
    ads1299_mcxa153_context_t ctx = {
        .spi = BOARD_ADS1299_SPI,
        .gpio = BOARD_ADS1299_GPIO,
        .ports = {
            BOARD_ADS1299_CS_PORT,
            BOARD_ADS1299_RESET_PORT,
            BOARD_ADS1299_PWDN_PORT,
            BOARD_ADS1299_START_PORT,
            BOARD_ADS1299_DRDY_PORT,
        },
        .pins = {
            BOARD_ADS1299_CS_PIN,
            BOARD_ADS1299_RESET_PIN,
            BOARD_ADS1299_PWDN_PIN,
            BOARD_ADS1299_START_PIN,
            BOARD_ADS1299_DRDY_PIN,
        },
        .source_hz = BOARD_ADS1299_SPI_SOURCE_HZ,
        .spi_hz = BOARD_ADS1299_SPI_HZ,
        .poll_limit = BOARD_ADS1299_POLL_LIMIT,
        .delay_user = BOARD_ADS1299_DELAY_USER,
        .delay_us = BOARD_ADS1299_DELAY_US,
        .fault = 0,
    };
    return ctx;
}

#endif /* MCXA153_BOARD_ADS1299_CONFIG_H */
