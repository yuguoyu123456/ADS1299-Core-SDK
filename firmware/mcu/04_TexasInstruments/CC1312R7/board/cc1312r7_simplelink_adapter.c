/*
 * Optional TI SimpleLink / SysConfig adapter for CC1312R7.
 *
 * This translation unit is intentionally opt-in. Add it to a real TI Drivers
 * project after SysConfig has generated ti_drivers_config.h and set the
 * resource identifiers in board_ads1299_config.h to the generated indices.
 * The generic repository build does not compile this file, so host tests stay
 * independent of the TI SDK.
 *
 * The adapter owns CS in software. Configure the selected SPI instance as a
 * controller with no automatic chip-select, SPI Mode 1, 8-bit transfers and
 * MSB-first operation in SysConfig. ADS1299 register/protocol behavior remains
 * in the shared core.
 */

#include "board_ads1299_binding.h"

#if defined(CC1312R7_ADS1299_ENABLE_SIMPLELINK_ADAPTER)

#include <stddef.h>
#include <stdint.h>

#include <ti/drivers/GPIO.h>
#include <ti/drivers/SPI.h>
#include <ti/drivers/dpl/ClockP.h>
#include "ti_drivers_config.h"

typedef struct {
    SPI_Handle spi;
    cc1312r7_ads1299_board_config_t cfg;
} cc1312r7_ads1299_simplelink_context_t;

static cc1312r7_ads1299_simplelink_context_t g_ads1299_ctx;

static int simplelink_spi_transfer(void *user,
                                   const uint8_t *tx,
                                   uint8_t *rx,
                                   size_t len)
{
    cc1312r7_ads1299_simplelink_context_t *ctx =
        (cc1312r7_ads1299_simplelink_context_t *)user;
    SPI_Transaction transaction;

    if (ctx == NULL || ctx->spi == NULL || len == 0u) {
        return -1;
    }

    SPI_Transaction_init(&transaction);
    transaction.count = len;
    transaction.txBuf = (void *)tx;
    transaction.rxBuf = rx;

    return SPI_transfer(ctx->spi, &transaction) ? 0 : -1;
}

static int simplelink_pin_write(void *user, uint32_t pin, int level)
{
    (void)user;
    GPIO_write(pin, level ? 1u : 0u);
    return 0;
}

static int simplelink_pin_read(void *user, uint32_t pin, int *level)
{
    (void)user;
    if (level == NULL) {
        return -1;
    }
    *level = GPIO_read(pin) ? 1 : 0;
    return 0;
}

static void simplelink_delay_us(void *user, uint32_t delay_us)
{
    (void)user;
    ClockP_usleep(delay_us);
}

int cc1312r7_ads1299_sdk_make_hal(
    const cc1312r7_ads1299_board_config_t *cfg,
    ads1299_platform_hal_t *out_hal)
{
    SPI_Params params;

    if (cfg == NULL || out_hal == NULL ||
        !cc1312r7_ads1299_board_config_is_complete(cfg)) {
        return CC1312R7_ADS1299_BIND_ERR_CONFIG;
    }

    SPI_init();
    GPIO_init();

    SPI_Params_init(&params);
    params.bitRate = cfg->spi_hz;
    params.frameFormat = SPI_POL0_PHA1;
    params.mode = SPI_CONTROLLER;
    params.transferMode = SPI_MODE_BLOCKING;

    g_ads1299_ctx.spi = SPI_open((uint_least8_t)cfg->spi_instance, &params);
    if (g_ads1299_ctx.spi == NULL) {
        return CC1312R7_ADS1299_BIND_ERR_SDK;
    }
    g_ads1299_ctx.cfg = *cfg;

    /* Safe inactive/control defaults before the shared driver starts. */
    GPIO_write((uint_least8_t)cfg->pin_cs, 1u);
    GPIO_write((uint_least8_t)cfg->pin_start, 0u);
    GPIO_write((uint_least8_t)cfg->pin_reset, 1u);
    GPIO_write((uint_least8_t)cfg->pin_pwdn, 1u);

    out_hal->user = &g_ads1299_ctx;
    out_hal->spi_transfer = simplelink_spi_transfer;
    out_hal->pin_write = simplelink_pin_write;
    out_hal->pin_read = simplelink_pin_read;
    out_hal->delay_us = simplelink_delay_us;
    return CC1312R7_ADS1299_BIND_OK;
}

#endif /* CC1312R7_ADS1299_ENABLE_SIMPLELINK_ADAPTER */
