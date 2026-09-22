#ifndef BOARD_ADS1299_CONFIG_H
#define BOARD_ADS1299_CONFIG_H

#include <stdint.h>
#include "../mcux_adapter/ads1299_mcxa156_hal.h"

/*
 * MCXA156 ADS1299 board configuration contract.
 *
 * This is the one intended hardware-dependent edit point for a new board.
 * Values intentionally default to an incomplete/fail-closed configuration:
 * copy the LPSPI/GPIO/pin/clock values from the MCUXpresso project generated
 * for the board actually being used.  Do not edit the shared ADS1299 core.
 *
 * Signal index used by ports[] / pins[]:
 *   0 = CS, 1 = RESET, 2 = PWDN, 3 = START, 4 = DRDY
 */

#ifndef BOARD_ADS1299_LPSPI
#define BOARD_ADS1299_LPSPI ((LPSPI_Type *)0)
#endif

#ifndef BOARD_ADS1299_GPIO
#define BOARD_ADS1299_GPIO ((GPIO_Type *)0)
#endif

#ifndef BOARD_ADS1299_CS_PORT
#define BOARD_ADS1299_CS_PORT 0u
#endif
#ifndef BOARD_ADS1299_CS_PIN
#define BOARD_ADS1299_CS_PIN 0u
#endif
#ifndef BOARD_ADS1299_RESET_PORT
#define BOARD_ADS1299_RESET_PORT 0u
#endif
#ifndef BOARD_ADS1299_RESET_PIN
#define BOARD_ADS1299_RESET_PIN 0u
#endif
#ifndef BOARD_ADS1299_PWDN_PORT
#define BOARD_ADS1299_PWDN_PORT 0u
#endif
#ifndef BOARD_ADS1299_PWDN_PIN
#define BOARD_ADS1299_PWDN_PIN 0u
#endif
#ifndef BOARD_ADS1299_START_PORT
#define BOARD_ADS1299_START_PORT 0u
#endif
#ifndef BOARD_ADS1299_START_PIN
#define BOARD_ADS1299_START_PIN 0u
#endif
#ifndef BOARD_ADS1299_DRDY_PORT
#define BOARD_ADS1299_DRDY_PORT 0u
#endif
#ifndef BOARD_ADS1299_DRDY_PIN
#define BOARD_ADS1299_DRDY_PIN 0u
#endif

#ifndef BOARD_ADS1299_LPSPI_SOURCE_HZ
#define BOARD_ADS1299_LPSPI_SOURCE_HZ 0u
#endif

#ifndef BOARD_ADS1299_SPI_HZ
#define BOARD_ADS1299_SPI_HZ 1000000u
#endif

#ifndef BOARD_ADS1299_POLL_LIMIT
#define BOARD_ADS1299_POLL_LIMIT 100000u
#endif

/* Supply the project's real microsecond delay callback. */
#ifndef BOARD_ADS1299_DELAY_USER
#define BOARD_ADS1299_DELAY_USER ((void *)0)
#endif
#ifndef BOARD_ADS1299_DELAY_US
#define BOARD_ADS1299_DELAY_US ((void (*)(void *, uint32_t))0)
#endif

/* Set to 1 only after checking the selected pins against board/pinmap.md. */
#ifndef BOARD_ADS1299_WIRING_REVIEWED
#define BOARD_ADS1299_WIRING_REVIEWED 0
#endif

static inline int board_ads1299_config_valid(void)
{
    if (!BOARD_ADS1299_WIRING_REVIEWED) return 0;
    if (BOARD_ADS1299_LPSPI == (LPSPI_Type *)0) return 0;
    if (BOARD_ADS1299_GPIO == (GPIO_Type *)0) return 0;
    if (BOARD_ADS1299_LPSPI_SOURCE_HZ == 0u) return 0;
    if (BOARD_ADS1299_SPI_HZ == 0u) return 0;
    if (BOARD_ADS1299_DELAY_US == 0) return 0;
    return 1;
}

static inline ads1299_mcxa156_context_t board_ads1299_context(void)
{
    ads1299_mcxa156_context_t ctx = {
        .spi = BOARD_ADS1299_LPSPI,
        .gpio = BOARD_ADS1299_GPIO,
        .ports = {
            BOARD_ADS1299_CS_PORT,
            BOARD_ADS1299_RESET_PORT,
            BOARD_ADS1299_PWDN_PORT,
            BOARD_ADS1299_START_PORT,
            BOARD_ADS1299_DRDY_PORT
        },
        .pins = {
            BOARD_ADS1299_CS_PIN,
            BOARD_ADS1299_RESET_PIN,
            BOARD_ADS1299_PWDN_PIN,
            BOARD_ADS1299_START_PIN,
            BOARD_ADS1299_DRDY_PIN
        },
        .source_hz = BOARD_ADS1299_LPSPI_SOURCE_HZ,
        .spi_hz = BOARD_ADS1299_SPI_HZ,
        .poll_limit = BOARD_ADS1299_POLL_LIMIT,
        .delay_user = BOARD_ADS1299_DELAY_USER,
        .delay_us = BOARD_ADS1299_DELAY_US,
        .fault = 0
    };
    return ctx;
}

#endif
