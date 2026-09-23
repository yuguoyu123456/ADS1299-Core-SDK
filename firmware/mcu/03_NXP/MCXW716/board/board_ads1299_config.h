#ifndef BOARD_ADS1299_CONFIG_H
#define BOARD_ADS1299_CONFIG_H

#include <stdint.h>
#include "../mcux_adapter/ads1299_mcxw716_hal.h"

/*
 * MCXW716 ADS1299 board configuration.
 *
 * This is the one board-dependent file intended for initial bring-up.
 * Keep shared ADS1299 register/profile logic out of this file.
 *
 * The defaults deliberately fail closed: this repository does not guess
 * board routing, LPSPI instance, clocks, or GPIO pins. Fill the macros from
 * the selected MCUXpresso board project / schematic, verify the wiring, then
 * set BOARD_ADS1299_WIRING_REVIEWED to 1.
 */
#ifndef BOARD_ADS1299_WIRING_REVIEWED
#define BOARD_ADS1299_WIRING_REVIEWED 0
#endif

#ifndef BOARD_ADS1299_LPSPI
#define BOARD_ADS1299_LPSPI ((LPSPI_Type *)0)
#endif
#ifndef BOARD_ADS1299_SOURCE_CLOCK_HZ
#define BOARD_ADS1299_SOURCE_CLOCK_HZ 0u
#endif
#ifndef BOARD_ADS1299_BAUD_HZ
#define BOARD_ADS1299_BAUD_HZ 1000000u
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

/* Supply a real microsecond delay from the MCUXpresso board/application. */
#ifndef BOARD_ADS1299_DELAY_US
#define BOARD_ADS1299_DELAY_US ((void (*)(uint32_t))0)
#endif

static inline int board_ads1299_config_valid(void)
{
    return (BOARD_ADS1299_WIRING_REVIEWED == 1) &&
           (BOARD_ADS1299_LPSPI != (LPSPI_Type *)0) &&
           (BOARD_ADS1299_GPIO != (GPIO_Type *)0) &&
           (BOARD_ADS1299_SOURCE_CLOCK_HZ != 0u) &&
           (BOARD_ADS1299_BAUD_HZ != 0u) &&
           (BOARD_ADS1299_DELAY_US != (void (*)(uint32_t))0);
}

static inline ads1299_mcxw716_hal_context_t board_ads1299_context(void)
{
    ads1299_mcxw716_hal_context_t ctx = {
        .lpspi = BOARD_ADS1299_LPSPI,
        .source_clock_hz = BOARD_ADS1299_SOURCE_CLOCK_HZ,
        .baud_hz = BOARD_ADS1299_BAUD_HZ,
        .cs = { BOARD_ADS1299_GPIO, BOARD_ADS1299_CS_PORT, BOARD_ADS1299_CS_PIN },
        .reset = { BOARD_ADS1299_GPIO, BOARD_ADS1299_RESET_PORT, BOARD_ADS1299_RESET_PIN },
        .pwdn = { BOARD_ADS1299_GPIO, BOARD_ADS1299_PWDN_PORT, BOARD_ADS1299_PWDN_PIN },
        .start = { BOARD_ADS1299_GPIO, BOARD_ADS1299_START_PORT, BOARD_ADS1299_START_PIN },
        .drdy = { BOARD_ADS1299_GPIO, BOARD_ADS1299_DRDY_PORT, BOARD_ADS1299_DRDY_PIN },
        .delay_us = BOARD_ADS1299_DELAY_US,
    };
    return ctx;
}

#endif /* BOARD_ADS1299_CONFIG_H */
