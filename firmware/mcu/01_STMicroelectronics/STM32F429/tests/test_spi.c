#include <assert.h>
#include <string.h>
#include "ads1299_spi.h"

static unsigned calls;
static int fake_spi(void *u, const uint8_t *tx, uint8_t *rx, size_t n) {
    (void)u; ++calls; if (rx && tx) memcpy(rx, tx, n); return 0;
}
static void fake_write(void *u, ads1299_platform_pin_t p, int v) {(void)u;(void)p;(void)v;}
static int fake_read(void *u, ads1299_platform_pin_t p) {(void)u;(void)p;return 1;}
static void fake_delay(void *u, uint32_t n) {(void)u;(void)n;}

void test_port_spi(void) {
    ads1299_platform_hal_t hal = {0, fake_spi, fake_write, fake_read, fake_delay};
    ads1299_mcu_port_t ctx; assert(ads1299_mcu_port_init(&ctx, &hal) == 0);
    ads1299_port_t port = ads1299_mcu_make_port(&ctx);
    uint8_t tx[2] = {0x12, 0x34}, rx[2] = {0};
    assert(port.spi_transfer(port.user, tx, rx, 2) == 0);
    assert(calls == 1 && memcmp(tx, rx, 2) == 0);
    assert(ads1299_mcu_spi_write(&ctx, tx, 2) == 0);
    assert(ads1299_mcu_spi_read(&ctx, rx, 2) == 0);
    assert(calls == 3);
}
