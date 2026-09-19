#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "ads1299_spi.h"

static int fake_spi(void *user, const uint8_t *tx, uint8_t *rx, size_t len) {
    (void)user;
    if (rx) {
        for (size_t i = 0; i < len; ++i) rx[i] = tx ? tx[i] : 0xA5u;
    }
    return 0;
}

static void fake_write(void *user, ads1299_platform_pin_t pin, int level) {
    (void)user;
    (void)pin;
    (void)level;
}

static int fake_read(void *user, ads1299_platform_pin_t pin) {
    (void)user;
    (void)pin;
    return 0;
}

static void fake_delay(void *user, uint32_t us) {
    (void)user;
    (void)us;
}

static ads1299_platform_hal_t valid_hal(void) {
    ads1299_platform_hal_t hal;
    memset(&hal, 0, sizeof(hal));
    hal.spi_transfer = fake_spi;
    hal.pin_write = fake_write;
    hal.pin_read = fake_read;
    hal.delay_us = fake_delay;
    return hal;
}

int main(void) {
    ads1299_mcu_port_t ctx;
    ads1299_platform_hal_t hal = valid_hal();
    uint8_t tx[2] = {0x12u, 0x34u};
    uint8_t rx[2] = {0u, 0u};

    assert(ads1299_mcu_port_init(NULL, &hal) == -1);
    assert(ads1299_mcu_port_init(&ctx, NULL) == -1);

    {
        ads1299_platform_hal_t bad = hal;
        bad.spi_transfer = NULL;
        assert(ads1299_mcu_port_init(&ctx, &bad) == -1);
    }
    {
        ads1299_platform_hal_t bad = hal;
        bad.pin_write = NULL;
        assert(ads1299_mcu_port_init(&ctx, &bad) == -1);
    }
    {
        ads1299_platform_hal_t bad = hal;
        bad.pin_read = NULL;
        assert(ads1299_mcu_port_init(&ctx, &bad) == -1);
    }
    {
        ads1299_platform_hal_t bad = hal;
        bad.delay_us = NULL;
        assert(ads1299_mcu_port_init(&ctx, &bad) == -1);
    }

    assert(ads1299_mcu_port_init(&ctx, &hal) == 0);
    assert(ads1299_mcu_spi_transfer(NULL, tx, rx, sizeof(tx)) == -1);
    assert(ads1299_mcu_spi_transfer(&ctx, NULL, NULL, sizeof(tx)) == -1);
    assert(ads1299_mcu_spi_transfer(&ctx, tx, rx, 0u) == -1);
    assert(ads1299_mcu_spi_write(&ctx, NULL, sizeof(tx)) == -1);
    assert(ads1299_mcu_spi_read(&ctx, NULL, sizeof(rx)) == -1);

    assert(ads1299_mcu_spi_transfer(&ctx, tx, rx, sizeof(tx)) == 0);
    assert(rx[0] == tx[0] && rx[1] == tx[1]);
    assert(ads1299_mcu_spi_write(&ctx, tx, sizeof(tx)) == 0);
    assert(ads1299_mcu_spi_read(&ctx, rx, sizeof(rx)) == 0);
    assert(rx[0] == 0xA5u && rx[1] == 0xA5u);

    return 0;
}
