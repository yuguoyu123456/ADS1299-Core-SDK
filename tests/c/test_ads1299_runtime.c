#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "ads1299_runtime.h"

typedef struct {
    uint8_t tx_log[64];
    size_t spi_calls;
    uint32_t delay_total;
    int start_level;
    int pwdn_level;
    int drdy_level;
    uint8_t rx_fill;
} mock_t;

static int spi_transfer(void *user, const uint8_t *tx, uint8_t *rx, size_t len) {
    mock_t *m = (mock_t *)user;
    if (tx && len) m->tx_log[m->spi_calls] = tx[0];
    if (rx) memset(rx, m->rx_fill, len);
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
    assert(dev.standby_mode == 1u);
    assert(m.tx_log[before] == ADS1299_CMD_STANDBY);

    before = m.spi_calls;
    assert(ads1299_start(&dev) == ADS1299_ESTATE);
    assert(ads1299_sdatac(&dev) == ADS1299_ESTATE);
    assert(m.spi_calls == before);

    uint8_t tmp = 0u;
    assert(ads1299_read_register(&dev, ADS1299_REG_CONFIG1, &tmp) == ADS1299_ESTATE);
    assert(m.spi_calls == before);

    uint32_t delay_before = m.delay_total;
    assert(ads1299_wakeup(&dev) == ADS1299_OK);
    assert(dev.standby_mode == 0u);
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
    assert(ads1299_safe_write_register(&dev, ADS1299_REG_CONFIG1, 0xF6u,
                                       ADS1299_VARIANT_8CH, &written) == ADS1299_OK);
    assert(written == 0xF6u);
    assert(m.tx_log[before] == ADS1299_CMD_SDATAC);
    assert(m.tx_log[before + 1u] ==
           (uint8_t)(ADS1299_CMD_WREG | ADS1299_REG_CONFIG1));

    before = m.spi_calls;
    assert(ads1299_safe_write_register(&dev, ADS1299_REG_CONFIG1, 0xFFu,
                                       ADS1299_VARIANT_8CH, NULL) == ADS1299_EINVAL);
    assert(ads1299_safe_write_register(&dev, ADS1299_REG_CONFIG2, 0xC2u,
                                       ADS1299_VARIANT_8CH, NULL) == ADS1299_EINVAL);
    assert(ads1299_safe_write_register(&dev, ADS1299_REG_CH1SET,
                                       ADS1299_GAIN_DO_NOT_USE,
                                       ADS1299_VARIANT_8CH, NULL) == ADS1299_EINVAL);
    assert(m.spi_calls == before);

    const uint8_t requested[2] = {0xF6u, 0xD5u};
    uint8_t sanitized[2] = {0u, 0u};
    assert(ads1299_safe_write_registers(&dev, ADS1299_REG_CONFIG1, requested, 2u,
                                        ADS1299_VARIANT_8CH, sanitized) == ADS1299_OK);
    assert(sanitized[0] == 0xF6u);
    assert(sanitized[1] == 0xD5u);

    const uint8_t bad_bulk[2] = {0xF6u, 0xC2u};
    before = m.spi_calls;
    assert(ads1299_safe_write_registers(&dev, ADS1299_REG_CONFIG1, bad_bulk, 2u,
                                        ADS1299_VARIANT_8CH, NULL) == ADS1299_EINVAL);
    assert(m.spi_calls == before);

    /* Strict mode never fixes caller mistakes. TI Rev. C's application example
     * contains LOFF=0x13 for "dc lead-off" even though bit4 is reserved=0 and
     * FLEAD_OFF=11 selects fDR/4. Strict mode catches the byte before SPI. */
    before = m.spi_calls;
    assert(ads1299_strict_write_register(&dev, ADS1299_REG_LOFF, 0x13u,
                                         ADS1299_VARIANT_8CH) == ADS1299_EINVAL);
    assert(m.spi_calls == before);

    /* Normalizing mode is intentionally different: it clears reserved bit4 and
     * reports the actual WREG byte, making the semantic change visible. */
    written = 0xFFu;
    assert(ads1299_safe_write_register(&dev, ADS1299_REG_LOFF, 0x13u,
                                       ADS1299_VARIANT_8CH, &written) == ADS1299_OK);
    assert(written == 0x03u);

    before = m.spi_calls;
    assert(ads1299_strict_write_register(&dev, ADS1299_REG_CONFIG1, 0xF6u,
                                         ADS1299_VARIANT_8CH) == ADS1299_OK);
    assert(m.spi_calls > before);

    const uint8_t strict_good[2] = {0xF6u, 0xD5u};
    assert(ads1299_strict_write_registers(&dev, ADS1299_REG_CONFIG1, strict_good, 2u,
                                          ADS1299_VARIANT_8CH) == ADS1299_OK);
    const uint8_t strict_bad[2] = {0xF6u, 0xC2u};
    before = m.spi_calls;
    assert(ads1299_strict_write_registers(&dev, ADS1299_REG_CONFIG1, strict_bad, 2u,
                                          ADS1299_VARIANT_8CH) == ADS1299_EINVAL);
    assert(m.spi_calls == before);

    before = m.spi_calls;
    assert(ads1299_safe_update_register_bits(&dev, ADS1299_REG_CONFIG1,
                                             0x08u, 0x08u,
                                             ADS1299_VARIANT_8CH, NULL) == ADS1299_EINVAL);
    assert(m.spi_calls == before);

    before = m.spi_calls;
    assert(ads1299_safe_write_register(&dev, ADS1299_REG_ID, 0xFFu,
                                       ADS1299_VARIANT_8CH, NULL) == ADS1299_EINVAL);
    assert(m.spi_calls == before);

    m.rx_fill = 0x1Cu;
    ads1299_device_id_t id = {0};
    assert(ads1299_read_device_id(&dev, &id) == ADS1299_OK);
    assert(id.is_ads1299_family == 1u && id.channel_count == 4u);
    assert(dev.channel_count == 4u);
    before = m.spi_calls;
    assert(ads1299_set_channel(&dev, 5u, ADS1299_GAIN_24,
                               ADS1299_MUX_NORMAL, 0, 0) == ADS1299_EINVAL);
    assert(m.spi_calls == before);

    puts("ADS1299 runtime/state/strict-normalize/field tests passed");
    return 0;
}
