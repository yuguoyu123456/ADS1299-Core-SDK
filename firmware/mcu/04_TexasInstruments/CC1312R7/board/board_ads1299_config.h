#ifndef CC1312R7_BOARD_ADS1299_CONFIG_H
#define CC1312R7_BOARD_ADS1299_CONFIG_H

#include <stdint.h>

/*
 * Single hardware-dependent configuration point for the CC1312R7 port.
 *
 * These values intentionally describe board resources rather than ADS1299
 * registers.  A future TI SimpleLink binding should translate them to the
 * generated Board/ti_drivers_config symbols used by the selected LaunchPad
 * project.  Keep ADS1299 protocol configuration in the shared core.
 *
 * TEMPLATE STATUS: the repository does not yet claim a build-verified
 * CC1312R7 reference-board binding.  Replace the *_UNASSIGNED values only
 * after checking the exact board revision and SysConfig output.
 */

typedef int32_t cc1312r7_board_resource_t;

#define CC1312R7_BOARD_RESOURCE_UNASSIGNED ((cc1312r7_board_resource_t)-1)

typedef struct {
    cc1312r7_board_resource_t spi_instance;
    cc1312r7_board_resource_t pin_cs;
    cc1312r7_board_resource_t pin_drdy;
    cc1312r7_board_resource_t pin_reset;
    cc1312r7_board_resource_t pin_pwdn;
    cc1312r7_board_resource_t pin_start;
    uint32_t spi_hz;
} cc1312r7_ads1299_board_config_t;

/* Conservative bring-up clock. The TI SDK binding must configure SPI Mode 1
 * (CPOL=0, CPHA=1), MSB first; this header deliberately cannot hide that
 * transport invariant in an ADS1299 register literal. */
#define CC1312R7_ADS1299_DEFAULT_SPI_HZ 1000000u

static inline cc1312r7_ads1299_board_config_t
cc1312r7_ads1299_board_config_default(void)
{
    cc1312r7_ads1299_board_config_t cfg;
    cfg.spi_instance = CC1312R7_BOARD_RESOURCE_UNASSIGNED;
    cfg.pin_cs = CC1312R7_BOARD_RESOURCE_UNASSIGNED;
    cfg.pin_drdy = CC1312R7_BOARD_RESOURCE_UNASSIGNED;
    cfg.pin_reset = CC1312R7_BOARD_RESOURCE_UNASSIGNED;
    cfg.pin_pwdn = CC1312R7_BOARD_RESOURCE_UNASSIGNED;
    cfg.pin_start = CC1312R7_BOARD_RESOURCE_UNASSIGNED;
    cfg.spi_hz = CC1312R7_ADS1299_DEFAULT_SPI_HZ;
    return cfg;
}

static inline int
cc1312r7_ads1299_board_config_is_complete(
    const cc1312r7_ads1299_board_config_t *cfg)
{
    return cfg != 0 &&
           cfg->spi_instance != CC1312R7_BOARD_RESOURCE_UNASSIGNED &&
           cfg->pin_cs != CC1312R7_BOARD_RESOURCE_UNASSIGNED &&
           cfg->pin_drdy != CC1312R7_BOARD_RESOURCE_UNASSIGNED &&
           cfg->pin_reset != CC1312R7_BOARD_RESOURCE_UNASSIGNED &&
           cfg->pin_pwdn != CC1312R7_BOARD_RESOURCE_UNASSIGNED &&
           cfg->pin_start != CC1312R7_BOARD_RESOURCE_UNASSIGNED &&
           cfg->spi_hz > 0u && cfg->spi_hz <= 4000000u;
}

#endif
