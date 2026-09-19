#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include "ads1299_spi.h"

static int fake_spi(void *user, const uint8_t *tx, uint8_t *rx, size_t len)
{
    (void)user;
    (void)tx;
    (void)rx;
    (void)len;
    return 0;
}

static void fake_write(void *user, ads1299_platform_pin_t pin, int level)
{
    (void)user;
    (void)pin;
    (void)level;
}

static int fake_read(void *user, ads1299_platform_pin_t pin)
{
    (void)user;
    (void)pin;
    return 1;
}

static void fake_delay(void *user, uint32_t us)
{
    (void)user;
    (void)us;
}

void test_port_validation(void)
{
    ads1299_mcu_port_t ctx;
    ads1299_platform_hal_t hal = {
        .user = NULL,
        .spi_transfer = fake_spi,
        .pin_write = fake_write,
        .pin_read = fake_read,
        .delay_us = fake_delay,
    };
    uint8_t byte = 0u;

    /* A usable platform contract must provide every required callback. */
    assert(ads1299_mcu_port_init(NULL, &hal) != 0);
    assert(ads1299_mcu_port_init(&ctx, NULL) != 0);

    hal.spi_transfer = NULL;
    assert(ads1299_mcu_port_init(&ctx, &hal) != 0);
    hal.spi_transfer = fake_spi;

    hal.pin_write = NULL;
    assert(ads1299_mcu_port_init(&ctx, &hal) != 0);
    hal.pin_write = fake_write;

    hal.pin_read = NULL;
    assert(ads1299_mcu_port_init(&ctx, &hal) != 0);
    hal.pin_read = fake_read;

    hal.delay_us = NULL;
    assert(ads1299_mcu_port_init(&ctx, &hal) != 0);
    hal.delay_us = fake_delay;

    assert(ads1299_mcu_port_init(&ctx, &hal) == 0);

    /* Reject malformed transfers before they can reach a vendor HAL. */
    assert(ads1299_mcu_spi_transfer(NULL, &byte, NULL, 1u) != 0);
    assert(ads1299_mcu_spi_transfer(&ctx, NULL, NULL, 1u) != 0);
    assert(ads1299_mcu_spi_transfer(&ctx, &byte, NULL, 0u) != 0);
    assert(ads1299_mcu_spi_write(&ctx, NULL, 1u) != 0);
    assert(ads1299_mcu_spi_read(&ctx, NULL, 1u) != 0);
}
