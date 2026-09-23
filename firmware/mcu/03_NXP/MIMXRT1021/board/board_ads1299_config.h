#ifndef BOARD_ADS1299_CONFIG_H
#define BOARD_ADS1299_CONFIG_H

/*
 * Single board-dependent configuration point for ADS1299 bring-up.
 *
 * Fill these values from the MCUXpresso-generated board/pin configuration for
 * the actual MIMXRT1021 board.  This file deliberately fails closed: no LPSPI
 * instance, GPIO or clock is guessed by the SDK.
 */

#include "../mcux_adapter/ads1299_mimxrt1021_hal.h"

#ifndef BOARD_ADS1299_WIRING_REVIEWED
#define BOARD_ADS1299_WIRING_REVIEWED 0
#endif

#ifndef BOARD_ADS1299_LPSPI
#define BOARD_ADS1299_LPSPI ((LPSPI_Type *)0)
#endif
#ifndef BOARD_ADS1299_LPSPI_SOURCE_CLOCK_HZ
#define BOARD_ADS1299_LPSPI_SOURCE_CLOCK_HZ 0u
#endif
#ifndef BOARD_ADS1299_SPI_BAUD_HZ
#define BOARD_ADS1299_SPI_BAUD_HZ 1000000u
#endif

#ifndef BOARD_ADS1299_CS_GPIO
#define BOARD_ADS1299_CS_GPIO ((GPIO_Type *)0)
#endif
#ifndef BOARD_ADS1299_CS_PIN
#define BOARD_ADS1299_CS_PIN 0u
#endif
#ifndef BOARD_ADS1299_RESET_GPIO
#define BOARD_ADS1299_RESET_GPIO ((GPIO_Type *)0)
#endif
#ifndef BOARD_ADS1299_RESET_PIN
#define BOARD_ADS1299_RESET_PIN 0u
#endif
#ifndef BOARD_ADS1299_PWDN_GPIO
#define BOARD_ADS1299_PWDN_GPIO ((GPIO_Type *)0)
#endif
#ifndef BOARD_ADS1299_PWDN_PIN
#define BOARD_ADS1299_PWDN_PIN 0u
#endif
#ifndef BOARD_ADS1299_START_GPIO
#define BOARD_ADS1299_START_GPIO ((GPIO_Type *)0)
#endif
#ifndef BOARD_ADS1299_START_PIN
#define BOARD_ADS1299_START_PIN 0u
#endif
#ifndef BOARD_ADS1299_DRDY_GPIO
#define BOARD_ADS1299_DRDY_GPIO ((GPIO_Type *)0)
#endif
#ifndef BOARD_ADS1299_DRDY_PIN
#define BOARD_ADS1299_DRDY_PIN 0u
#endif

/* Define this to the project's real microsecond delay function. */
#ifndef BOARD_ADS1299_DELAY_US
#define BOARD_ADS1299_DELAY_US ((void (*)(uint32_t))0)
#endif

static inline int board_ads1299_config_valid(void)
{
    return (BOARD_ADS1299_WIRING_REVIEWED == 1) &&
           (BOARD_ADS1299_LPSPI != (LPSPI_Type *)0) &&
           (BOARD_ADS1299_LPSPI_SOURCE_CLOCK_HZ != 0u) &&
           (BOARD_ADS1299_SPI_BAUD_HZ != 0u) &&
           (BOARD_ADS1299_CS_GPIO != (GPIO_Type *)0) &&
           (BOARD_ADS1299_RESET_GPIO != (GPIO_Type *)0) &&
           (BOARD_ADS1299_PWDN_GPIO != (GPIO_Type *)0) &&
           (BOARD_ADS1299_START_GPIO != (GPIO_Type *)0) &&
           (BOARD_ADS1299_DRDY_GPIO != (GPIO_Type *)0) &&
           (BOARD_ADS1299_DELAY_US != (void (*)(uint32_t))0);
}

static inline ads1299_mimxrt1021_hal_context_t board_ads1299_context(void)
{
    ads1299_mimxrt1021_hal_context_t ctx = {
        .lpspi = BOARD_ADS1299_LPSPI,
        .source_clock_hz = BOARD_ADS1299_LPSPI_SOURCE_CLOCK_HZ,
        .baud_hz = BOARD_ADS1299_SPI_BAUD_HZ,
        .cs = { BOARD_ADS1299_CS_GPIO, BOARD_ADS1299_CS_PIN },
        .reset = { BOARD_ADS1299_RESET_GPIO, BOARD_ADS1299_RESET_PIN },
        .pwdn = { BOARD_ADS1299_PWDN_GPIO, BOARD_ADS1299_PWDN_PIN },
        .start = { BOARD_ADS1299_START_GPIO, BOARD_ADS1299_START_PIN },
        .drdy = { BOARD_ADS1299_DRDY_GPIO, BOARD_ADS1299_DRDY_PIN },
        .delay_us = BOARD_ADS1299_DELAY_US,
    };
    return ctx;
}

#endif /* BOARD_ADS1299_CONFIG_H */
