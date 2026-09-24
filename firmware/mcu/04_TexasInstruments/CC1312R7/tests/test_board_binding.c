#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include "board_ads1299_binding.h"

static int g_sdk_result = CC1312R7_ADS1299_BIND_OK;
static int g_emit_complete_hal = 1;

static int fake_spi(void *user, const uint8_t *tx, uint8_t *rx, size_t len)
{
    (void)user;
    (void)tx;
    (void)rx;
    (void)len;
    return 0;
}

static void fake_write(void *user, int pin, int level)
{
    (void)user;
    (void)pin;
    (void)level;
}

static int fake_read(void *user, int pin)
{
    (void)user;
    (void)pin;
    return 0;
}

static void fake_delay(void *user, uint32_t us)
{
    (void)user;
    (void)us;
}

/* Strong test definition overrides the weak repository SDK seam. */
int cc1312r7_ads1299_sdk_make_hal(
    const cc1312r7_ads1299_board_config_t *cfg,
    ads1299_platform_hal_t *out_hal)
{
    (void)cfg;
    if (g_sdk_result != CC1312R7_ADS1299_BIND_OK) {
        return g_sdk_result;
    }
    out_hal->user = (void *)(uintptr_t)0x1234u;
    out_hal->spi_transfer = fake_spi;
    out_hal->pin_write = fake_write;
    out_hal->pin_read = fake_read;
    out_hal->delay_us = g_emit_complete_hal ? fake_delay : NULL;
    return CC1312R7_ADS1299_BIND_OK;
}

static cc1312r7_ads1299_board_config_t complete_config(void)
{
    cc1312r7_ads1299_board_config_t cfg =
        cc1312r7_ads1299_board_config_default();
    cfg.spi_instance = 0;
    cfg.pin_cs = 1;
    cfg.pin_drdy = 2;
    cfg.pin_reset = 3;
    cfg.pin_pwdn = 4;
    cfg.pin_start = 5;
    return cfg;
}

static void assert_hal_cleared(const ads1299_platform_hal_t *hal)
{
    assert(hal->user == NULL);
    assert(hal->spi_transfer == NULL);
    assert(hal->pin_write == NULL);
    assert(hal->pin_read == NULL);
    assert(hal->delay_us == NULL);
}

int main(void)
{
    cc1312r7_ads1299_board_config_t cfg = complete_config();
    ads1299_platform_hal_t hal = {0};

    assert(cc1312r7_ads1299_make_hal(NULL, &hal) ==
           CC1312R7_ADS1299_BIND_ERR_ARGUMENT);
    assert(cc1312r7_ads1299_make_hal(&cfg, NULL) ==
           CC1312R7_ADS1299_BIND_ERR_ARGUMENT);

    cfg.pin_cs = CC1312R7_BOARD_RESOURCE_UNASSIGNED;
    assert(cc1312r7_ads1299_make_hal(&cfg, &hal) ==
           CC1312R7_ADS1299_BIND_ERR_CONFIG);

    cfg = complete_config();
    g_sdk_result = -99;
    hal.user = (void *)(uintptr_t)1u;
    hal.spi_transfer = fake_spi;
    assert(cc1312r7_ads1299_make_hal(&cfg, &hal) ==
           CC1312R7_ADS1299_BIND_ERR_SDK);
    assert_hal_cleared(&hal);

    g_sdk_result = CC1312R7_ADS1299_BIND_OK;
    g_emit_complete_hal = 0;
    assert(cc1312r7_ads1299_make_hal(&cfg, &hal) ==
           CC1312R7_ADS1299_BIND_ERR_SDK);
    assert_hal_cleared(&hal);

    g_emit_complete_hal = 1;
    assert(cc1312r7_ads1299_make_hal(&cfg, &hal) ==
           CC1312R7_ADS1299_BIND_OK);
    assert(hal.user != NULL);
    assert(hal.spi_transfer == fake_spi);
    assert(hal.pin_write == fake_write);
    assert(hal.pin_read == fake_read);
    assert(hal.delay_us == fake_delay);

    return 0;
}
