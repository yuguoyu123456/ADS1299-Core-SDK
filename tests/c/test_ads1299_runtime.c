#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "ads1299_runtime.h"

typedef struct {
    uint8_t tx_log[32];
    size_t spi_calls;
    uint32_t delay_total;
    int start_level;
    int pwdn_level;
    int drdy_level;
} mock_t;

static int spi_transfer(void *user, const uint8_t *tx, uint8_t *rx, size_t len) {
    mock_t *m = (mock_t *)user;
    if (tx && len) m->tx_log[m->spi_calls] = tx[0];
    if (rx) memset(rx, 0, len);
    m->spi_calls++;
    return 0;
}
static void gpio_ignore(void *user, int level) { (void)user; (void)level; }
static void start_write(void *user, int level) { ((mock_t *)user)->start_level = level; }
static void pwdn_write(void *user, int level) { ((mock_t *)user)->pwdn_level = level; }
static int drdy_read(void *user) { return ((mock_t *)user)->drdy_level; }
static void delay_us(void *user, uint32_t us) { ((mock_t *)user)->delay_total += us; }

static ads1299_t make_dev(mock_t *m) {
    ads1299_port_t port = {
        .user = m,
        .spi_transfer = spi_transfer,
        .cs_write = gpio_ignore,
        .reset_write = gpio_ignore,
        .pwdn_write = pwdn_write,
        .start_write = start_write,
        .drdy_read = drdy_read,
        .delay_us = delay_us,
    };
    ads1299_t dev;
    assert(ads1299_init(&dev, &port) == ADS1299_OK);
    return dev;
}

int main(void) {
    mock_t m = {0};
    m.drdy_level = 1;
    ads1299_t dev = make_dev(&m);

    size_t before = m.spi_calls;
    assert(ads1299_standby(&dev) == ADS1299_OK);
    assert(m.tx_log[before] == ADS1299_CMD_STANDBY);
    before = m.spi_calls;
    uint32_t delay_before = m.delay_total;
    assert(ads1299_wakeup(&dev) == ADS1299_OK);
    assert(m.tx_log[before] == ADS1299_CMD_WAKEUP);
    assert(m.delay_total >= delay_before + 4u);

    assert(ads1299_start_pin(&dev) == ADS1299_OK);
    assert(m.start_level == 1);
    assert(ads1299_stop_pin(&dev) == ADS1299_OK);
    assert(m.start_level == 0);

    dev.continuous_mode = 1u;
    assert(ads1299_set_power_down(&dev, 1) == ADS1299_OK);
    assert(m.pwdn_level == 0);
    assert(dev.continuous_mode == 0u);
    assert(ads1299_set_power_down(&dev, 0) == ADS1299_OK);
    assert(m.pwdn_level == 1);

    assert(ads1299_wait_drdy(&dev, 5u, 2u) == ADS1299_ETIMEOUT);
    m.drdy_level = 0;
    assert(ads1299_wait_drdy(&dev, 5u, 2u) == ADS1299_OK);

    uint8_t written = 0u;
    before = m.spi_calls;
    assert(ads1299_safe_write_register(&dev, ADS1299_REG_CONFIG1, 0xFFu,
                                       ADS1299_VARIANT_8CH, &written) == ADS1299_OK);
    assert(written == 0xF7u);
    assert(m.tx_log[before] == (uint8_t)(ADS1299_CMD_WREG | ADS1299_REG_CONFIG1));

    before = m.spi_calls;
    assert(ads1299_safe_write_register(&dev, ADS1299_REG_ID, 0xFFu,
                                       ADS1299_VARIANT_8CH, NULL) == ADS1299_EINVAL);
    assert(m.spi_calls == before);

    puts("ADS1299 runtime-control and safe-write tests passed");
    return 0;
}
