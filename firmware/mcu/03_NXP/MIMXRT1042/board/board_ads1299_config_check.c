/*
 * Compile-time smoke target for the MIMXRT1042 ADS1299 board contract.
 *
 * TEMPLATE / NOT BUILD-VERIFIED / NOT BOARD-VERIFIED.
 *
 * Add this translation unit to an evkmimxrt1040 MCUXpresso application while
 * first integrating the SDK.  A successful compile proves that the consuming
 * project supplied every required board-only symbol with types compatible
 * with the MIMXRT1042 HAL context.  It deliberately performs no ADS1299 I/O.
 */
#include "board_ads1299_config.h"
#include "../mcux_adapter/ads1299_mimxrt1042_hal.h"

extern void board_ads1299_delay_us(uint32_t us);

static ads1299_mimxrt1042_hal_context_t g_board_contract_check = {
    .lpspi = BOARD_ADS1299_LPSPI,
    .source_clock_hz = BOARD_ADS1299_LPSPI_CLOCK_HZ,
    .baud_hz = BOARD_ADS1299_SPI_BAUD_HZ,
    .cs = { BOARD_ADS1299_GPIO, BOARD_ADS1299_CS_PIN },
    .reset = { BOARD_ADS1299_GPIO, BOARD_ADS1299_RESET_PIN },
    .pwdn = { BOARD_ADS1299_GPIO, BOARD_ADS1299_PWDN_PIN },
    .start = { BOARD_ADS1299_GPIO, BOARD_ADS1299_START_PIN },
    .drdy = { BOARD_ADS1299_GPIO, BOARD_ADS1299_DRDY_PIN },
    .delay_us = board_ads1299_delay_us,
};

const ads1299_mimxrt1042_hal_context_t *board_ads1299_config_check(void)
{
    return &g_board_contract_check;
}
