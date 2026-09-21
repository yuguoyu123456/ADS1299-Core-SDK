#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "ads1299_spi.h"

static int stub_spi(void *user, const uint8_t *tx, uint8_t *rx, size_t len)
{
    (void)user;
    (void)tx;
    (void)rx;
    (void)len;
    return 0;
}

static void stub_write(void *user, ads1299_platform_pin_t pin, int level)
{
    (void)user;
    (void)pin;
    (void)level;
}

static int stub_read(void *user, ads1299_platform_pin_t pin)
{
    (void)user;
    (void)pin;
    return 1;
}

static void stub_delay(void *user, uint32_t us)
{
    (void)user;
    (void)us;
}

void test_port_init_validation(void)
{
    ads1299_mcu_port_t ctx;
    ads1299_platform_hal_t hal = {
        .user = (void *)(uintptr_t)0x1234u,
        .spi_transfer = stub_spi,
        .pin_write = stub_write,
        .pin_read = stub_read,
        .delay_us = stub_delay,
    };
    ads1299_platform_hal_t broken;

    memset(&ctx, 0, sizeof(ctx));
    assert(ads1299_mcu_port_init(NULL, &hal) == -1);
    assert(ads1299_mcu_port_init(&ctx, NULL) == -1);

    broken = hal;
    broken.spi_transfer = NULL;
    assert(ads1299_mcu_port_init(&ctx, &broken) == -1);
    broken = hal;
    broken.pin_write = NULL;
    assert(ads1299_mcu_port_init(&ctx, &broken) == -1);
    broken = hal;
    broken.pin_read = NULL;
    assert(ads1299_mcu_port_init(&ctx, &broken) == -1);
    broken = hal;
    broken.delay_us = NULL;
    assert(ads1299_mcu_port_init(&ctx, &broken) == -1);

    assert(ads1299_mcu_port_init(&ctx, &hal) == 0);
    assert(ctx.hal.user == hal.user);
    assert(ctx.hal.spi_transfer == hal.spi_transfer);
    assert(ctx.hal.pin_write == hal.pin_write);
    assert(ctx.hal.pin_read == hal.pin_read);
    assert(ctx.hal.delay_us == hal.delay_us);

    ads1299_port_t port = ads1299_mcu_make_port(&ctx);
    assert(port.user == &ctx);
    assert(port.spi_transfer != NULL);
    assert(port.cs_write != NULL);
    assert(port.reset_write != NULL);
    assert(port.pwdn_write != NULL);
    assert(port.start_write != NULL);
    assert(port.drdy_read != NULL);
    assert(port.delay_us != NULL);
}
