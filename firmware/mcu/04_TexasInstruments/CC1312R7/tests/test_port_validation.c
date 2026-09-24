#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include "ads1299_spi.h"

static int fake_spi(void *user, const uint8_t *tx, uint8_t *rx, size_t len)
{
    (void)user;
    (void)tx;
    (void)rx;
    return len > 0u ? 0 : -1;
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

int main(void)
{
    ads1299_mcu_port_t ctx;
    ads1299_platform_hal_t hal = {
        .user = NULL,
        .spi_transfer = fake_spi,
        .pin_write = fake_write,
        .pin_read = fake_read,
        .delay_us = fake_delay,
    };

    assert(ads1299_mcu_port_init(NULL, &hal) == -1);
    assert(ads1299_mcu_port_init(&ctx, NULL) == -1);

    ads1299_platform_hal_t invalid = hal;
    invalid.spi_transfer = NULL;
    assert(ads1299_mcu_port_init(&ctx, &invalid) == -1);
    invalid = hal;
    invalid.pin_write = NULL;
    assert(ads1299_mcu_port_init(&ctx, &invalid) == -1);
    invalid = hal;
    invalid.pin_read = NULL;
    assert(ads1299_mcu_port_init(&ctx, &invalid) == -1);
    invalid = hal;
    invalid.delay_us = NULL;
    assert(ads1299_mcu_port_init(&ctx, &invalid) == -1);

    assert(ads1299_mcu_port_init(&ctx, &hal) == 0);

    ads1299_port_t port = ads1299_mcu_make_port(&ctx);
    assert(port.user == &ctx);
    assert(port.spi_transfer != NULL);
    assert(port.cs_write != NULL);
    assert(port.reset_write != NULL);
    assert(port.pwdn_write != NULL);
    assert(port.start_write != NULL);
    assert(port.drdy_read != NULL);
    assert(port.delay_us != NULL);

    uint8_t byte = 0u;
    assert(ads1299_mcu_spi_transfer(NULL, &byte, NULL, 1u) == -1);
    assert(ads1299_mcu_spi_transfer(&ctx, NULL, NULL, 1u) == -1);
    assert(ads1299_mcu_spi_transfer(&ctx, &byte, NULL, 0u) == -1);
    assert(ads1299_mcu_spi_write(&ctx, NULL, 1u) == -1);
    assert(ads1299_mcu_spi_read(&ctx, NULL, 1u) == -1);
    assert(ads1299_mcu_spi_write(&ctx, &byte, 1u) == 0);
    assert(ads1299_mcu_spi_read(&ctx, &byte, 1u) == 0);

    return 0;
}
