#ifndef BOARD_ADS1299_CONFIG_H
#define BOARD_ADS1299_CONFIG_H

#include <stdint.h>
#include "../mcux_adapter/ads1299_mcxw236_hal.h"

/*
 * MCXW236 ADS1299 board configuration.
 *
 * This is intentionally the one hardware-dependent file a new board port
 * should edit.  Keep ADS1299 register/profile logic in the shared core.
 *
 * The defaults are fail-closed: no pins, peripheral instance, clock or delay
 * implementation are guessed.  After reviewing the board schematic and the
 * generated MCUXpresso pin/clock configuration, define the BOARD_ADS1299_*
 * macros below (normally from this file or the project compiler settings),
 * then set BOARD_ADS1299_WIRING_REVIEWED to 1.
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
#ifndef BOARD_ADS1299_DELAY_US
#define BOARD_ADS1299_DELAY_US ((void (*)(uint32_t))0)
#endif

#define BOARD_ADS1299_GPIO_DEFAULT ((ads1299_mcxw236_gpio_t){0})

#ifndef BOARD_ADS1299_CS
#define BOARD_ADS1299_CS BOARD_ADS1299_GPIO_DEFAULT
#endif
#ifndef BOARD_ADS1299_RESET
#define BOARD_ADS1299_RESET BOARD_ADS1299_GPIO_DEFAULT
#endif
#ifndef BOARD_ADS1299_PWDN
#define BOARD_ADS1299_PWDN BOARD_ADS1299_GPIO_DEFAULT
#endif
#ifndef BOARD_ADS1299_START
#define BOARD_ADS1299_START BOARD_ADS1299_GPIO_DEFAULT
#endif
#ifndef BOARD_ADS1299_DRDY
#define BOARD_ADS1299_DRDY BOARD_ADS1299_GPIO_DEFAULT
#endif

static inline int board_ads1299_gpio_valid(ads1299_mcxw236_gpio_t gpio)
{
    return gpio.gpio != (GPIO_Type *)0;
}

static inline int board_ads1299_config_valid(void)
{
    if (!BOARD_ADS1299_WIRING_REVIEWED) {
        return 0;
    }
    if ((BOARD_ADS1299_LPSPI == (LPSPI_Type *)0) ||
        (BOARD_ADS1299_SOURCE_CLOCK_HZ == 0u) ||
        (BOARD_ADS1299_BAUD_HZ == 0u) ||
        (BOARD_ADS1299_DELAY_US == (void (*)(uint32_t))0)) {
        return 0;
    }
    return board_ads1299_gpio_valid(BOARD_ADS1299_CS) &&
           board_ads1299_gpio_valid(BOARD_ADS1299_RESET) &&
           board_ads1299_gpio_valid(BOARD_ADS1299_PWDN) &&
           board_ads1299_gpio_valid(BOARD_ADS1299_START) &&
           board_ads1299_gpio_valid(BOARD_ADS1299_DRDY);
}

static inline ads1299_mcxw236_hal_context_t board_ads1299_context(void)
{
    ads1299_mcxw236_hal_context_t ctx = {
        .lpspi = BOARD_ADS1299_LPSPI,
        .source_clock_hz = BOARD_ADS1299_SOURCE_CLOCK_HZ,
        .baud_hz = BOARD_ADS1299_BAUD_HZ,
        .cs = BOARD_ADS1299_CS,
        .reset = BOARD_ADS1299_RESET,
        .pwdn = BOARD_ADS1299_PWDN,
        .start = BOARD_ADS1299_START,
        .drdy = BOARD_ADS1299_DRDY,
        .delay_us = BOARD_ADS1299_DELAY_US,
    };
    return ctx;
}

#endif /* BOARD_ADS1299_CONFIG_H */
