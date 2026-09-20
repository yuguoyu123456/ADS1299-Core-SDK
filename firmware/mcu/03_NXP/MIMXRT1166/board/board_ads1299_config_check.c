/*
 * Compile-only contract check for the MIMXRT1166 ADS1299 board configuration.
 *
 * The consuming official MCUXpresso project supplies the ADS1299_BOARD_*
 * macros. Including the single board contract here makes missing hardware
 * bindings fail during the integration smoke build rather than later at link
 * time or on hardware.
 *
 * This file intentionally contains no ADS1299 register policy and no invented
 * reference-board pin numbers.
 */
#include "board_ads1299_config.h"

#include <stdint.h>

/* Force the required macros to be valid C expressions of the expected broad
 * kind. GPIO/LPSPI SDK types vary by MCUXpresso release, so this check avoids
 * narrowing them to a repository-owned vendor type. */
static const uintptr_t ads1299_rt1166_config_contract[] = {
    (uintptr_t)(ADS1299_BOARD_LPSPI_BASE),
    (uintptr_t)(ADS1299_BOARD_CS_GPIO),
    (uintptr_t)(ADS1299_BOARD_RESET_GPIO),
    (uintptr_t)(ADS1299_BOARD_PWDN_GPIO),
    (uintptr_t)(ADS1299_BOARD_START_GPIO),
    (uintptr_t)(ADS1299_BOARD_DRDY_GPIO),
    (uintptr_t)(ADS1299_BOARD_CS_PIN),
    (uintptr_t)(ADS1299_BOARD_RESET_PIN),
    (uintptr_t)(ADS1299_BOARD_PWDN_PIN),
    (uintptr_t)(ADS1299_BOARD_START_PIN),
    (uintptr_t)(ADS1299_BOARD_DRDY_PIN),
    (uintptr_t)(ADS1299_BOARD_LPSPI_SOURCE_CLOCK_HZ),
    (uintptr_t)(ADS1299_BOARD_SPI_BAUD_HZ),
};

/* A callable symbol prevents an aggressive compile-only parent project from
 * treating the translation unit as intentionally empty. */
uintptr_t ads1299_rt1166_board_config_contract_word(void)
{
    uintptr_t value = 0u;
    unsigned int i;

    for (i = 0u;
         i < (unsigned int)(sizeof(ads1299_rt1166_config_contract) /
                            sizeof(ads1299_rt1166_config_contract[0]));
         ++i) {
        value ^= ads1299_rt1166_config_contract[i];
    }
    return value;
}
