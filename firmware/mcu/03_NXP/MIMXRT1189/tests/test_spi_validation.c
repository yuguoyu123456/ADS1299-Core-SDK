#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include "ads1299_spi.h"

static unsigned transfer_calls;

static int fake_spi(void *user, const uint8_t *tx, uint8_t *rx, size_t len)
{
    (void)user;
    (void)tx;
    (void)rx;
    (void)len;
    transfer_calls++;
    return 0;
}

static void fake_pin_write(void *user, ads1299_platform_pin_t pin, int level)
{
    (void)user;
    (void)pin;
    (void)level;
}

static int fake_pin_read(void *user, ads1299_platform_pin_t pin)
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

void test_spi_argument_validation(void)
{
    uint8_t byte = 0u;
    ads1299_platform_hal_t hal = {
        .user = NULL,
        .spi_transfer = fake_spi,
        .pin_write = fake_pin_write,
        .pin_read = fake_pin_read,
        .delay_us = fake_delay,
    };
    ads1299_mcu_port_t ctx;

    assert(ads1299_mcu_port_init(&ctx, &hal) == 0);
    transfer_calls = 0u;

    assert(ads1299_mcu_spi_transfer(NULL, &byte, &byte, 1u) == -1);
    assert(ads1299_mcu_spi_transfer(&ctx, NULL, NULL, 1u) == -1);
    assert(ads1299_mcu_spi_transfer(&ctx, &byte, &byte, 0u) == -1);
    assert(ads1299_mcu_spi_write(&ctx, NULL, 1u) == -1);
    assert(ads1299_mcu_spi_read(&ctx, NULL, 1u) == -1);
    assert(transfer_calls == 0u);

    assert(ads1299_mcu_spi_transfer(&ctx, &byte, NULL, 1u) == 0);
    assert(ads1299_mcu_spi_transfer(&ctx, NULL, &byte, 1u) == 0);
    assert(transfer_calls == 2u);
}
