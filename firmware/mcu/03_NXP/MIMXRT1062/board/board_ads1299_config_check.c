/*
 * Compile-only contract check for the MIMXRT1062 ADS1299 board layer.
 *
 * Add this source to an MCUXpresso EVKB build while bringing the platform up.
 * It intentionally performs no I/O: successful compilation proves that the
 * single board configuration header exposes the peripheral and every control
 * signal required by the RT1062 binding with SDK-compatible types.
 *
 * Validation status: TEMPLATE / compile contract only.  This is not evidence
 * of a successful MCUXpresso build or physical-board operation.
 */
#include "board_ads1299_config.h"

#include <stdint.h>

static LPSPI_Type *const board_ads1299_spi_contract = BOARD_ADS1299_LPSPI;
static GPIO_Type *const board_ads1299_gpio_contract[] = {
    BOARD_ADS1299_CS_GPIO,
    BOARD_ADS1299_RESET_GPIO,
    BOARD_ADS1299_PWDN_GPIO,
    BOARD_ADS1299_START_GPIO,
    BOARD_ADS1299_DRDY_GPIO,
};

static const uint32_t board_ads1299_pin_contract[] = {
    BOARD_ADS1299_CS_PIN,
    BOARD_ADS1299_RESET_PIN,
    BOARD_ADS1299_PWDN_PIN,
    BOARD_ADS1299_START_PIN,
    BOARD_ADS1299_DRDY_PIN,
};

/*
 * Reference this function from a temporary MCUXpresso smoke target if the
 * toolchain is configured to discard completely unreferenced translation
 * units.  Returning zero means only that the compile-time contract is sane.
 */
int board_ads1299_config_compile_check(void)
{
    uint32_t accumulator = BOARD_ADS1299_SPI_BAUD_HZ;
    unsigned int i;

    if (board_ads1299_spi_contract == NULL) {
        return -1;
    }

    for (i = 0u; i < (sizeof(board_ads1299_gpio_contract) /
                     sizeof(board_ads1299_gpio_contract[0])); ++i) {
        if (board_ads1299_gpio_contract[i] == NULL) {
            return -2;
        }
        accumulator ^= board_ads1299_pin_contract[i];
    }

    return (accumulator == 0u) ? -3 : 0;
}
