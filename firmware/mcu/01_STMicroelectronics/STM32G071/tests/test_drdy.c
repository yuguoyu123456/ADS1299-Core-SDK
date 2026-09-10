#include <assert.h>
#include "ads1299_drdy.h"

static int drdy_level = 1;

static int fake_spi(void *user, const uint8_t *tx, uint8_t *rx, size_t len) {
    (void)user;
    (void)tx;
    (void)rx;
    (void)len;
    return 0;
}

static void fake_write(void *user, ads1299_platform_pin_t pin, int level) {
    (void)user;
    (void)pin;
    (void)level;
}

static int fake_read(void *user, ads1299_platform_pin_t pin) {
    (void)user;
    assert(pin == ADS1299_PIN_DRDY);
    return drdy_level;
}

static void fake_delay(void *user, uint32_t us) {
    (void)user;
    (void)us;
}

void test_port_drdy_active_low(void) {
    ads1299_platform_hal_t hal = {0, fake_spi, fake_write, fake_read, fake_delay};
    ads1299_mcu_port_t ctx;
    ads1299_port_t port;

    assert(ads1299_mcu_port_init(&ctx, &hal) == 0);
    port = ads1299_mcu_make_port(&ctx);
    assert(port.drdy_read != 0);

    drdy_level = 1;
    assert(ads1299_mcu_data_ready(&ctx) == 0);
    assert(port.drdy_read(port.user) == 1);

    drdy_level = 0;
    assert(ads1299_mcu_data_ready(&ctx) == 1);
    assert(port.drdy_read(port.user) == 0);
}
