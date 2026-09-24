#include "board_ads1299_binding.h"

#include <stddef.h>

/*
 * Weak SDK seam for the CC1312R7 board binding.
 *
 * A real SimpleLink/SysConfig project supplies a strong definition of this
 * function and translates cfg resource identifiers to its generated SPI and
 * GPIO objects. Keeping that translation here prevents generated TI symbols
 * from leaking into the shared ADS1299 driver or examples.
 *
 * The repository template intentionally does not guess generated symbol names
 * or LaunchPad pin assignments. Until a project provides the strong adapter,
 * cc1312r7_ads1299_make_hal() returns CC1312R7_ADS1299_BIND_ERR_SDK.
 */
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
int cc1312r7_ads1299_sdk_make_hal(
    const cc1312r7_ads1299_board_config_t *cfg,
    ads1299_platform_hal_t *out_hal)
{
    (void)cfg;
    (void)out_hal;
    return CC1312R7_ADS1299_BIND_ERR_SDK;
}

int cc1312r7_ads1299_make_hal(
    const cc1312r7_ads1299_board_config_t *cfg,
    ads1299_platform_hal_t *out_hal)
{
    int rc;

    if (cfg == NULL || out_hal == NULL) {
        return CC1312R7_ADS1299_BIND_ERR_ARGUMENT;
    }

    if (!cc1312r7_ads1299_board_config_is_complete(cfg)) {
        return CC1312R7_ADS1299_BIND_ERR_CONFIG;
    }

    /* Fail closed: callers never receive stale callbacks after a failed bind. */
    out_hal->user = NULL;
    out_hal->spi_transfer = NULL;
    out_hal->pin_write = NULL;
    out_hal->pin_read = NULL;
    out_hal->delay_us = NULL;

    rc = cc1312r7_ads1299_sdk_make_hal(cfg, out_hal);
    if (rc != CC1312R7_ADS1299_BIND_OK) {
        return CC1312R7_ADS1299_BIND_ERR_SDK;
    }

    if (out_hal->spi_transfer == NULL || out_hal->pin_write == NULL ||
        out_hal->pin_read == NULL || out_hal->delay_us == NULL) {
        out_hal->user = NULL;
        out_hal->spi_transfer = NULL;
        out_hal->pin_write = NULL;
        out_hal->pin_read = NULL;
        out_hal->delay_us = NULL;
        return CC1312R7_ADS1299_BIND_ERR_SDK;
    }

    return CC1312R7_ADS1299_BIND_OK;
}
