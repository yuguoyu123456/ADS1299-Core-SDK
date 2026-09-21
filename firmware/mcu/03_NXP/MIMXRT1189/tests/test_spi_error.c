#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include "ads1299_spi.h"

static int failing_transfer(void *user, const uint8_t *tx, uint8_t *rx, size_t len)
{
    (void)user;
    (void)tx;
    (void)rx;
    (void)len;
    return -37;
}

static void pin_write_stub(void *user, ads1299_platform_pin_t pin, int level)
{
    (void)user;
    (void)pin;
    (void)level;
}

static int pin_read_stub(void *user, ads1299_platform_pin_t pin)
{
    (void)user;
    (void)pin;
    return 1;
}

static void delay_stub(void *user, uint32_t us)
{
    (void)user;
    (void)us;
}

int test_spi_error_propagation(void)
{
    ads1299_mcu_port_t ctx;
    const ads1299_platform_hal_t hal = {
        .user = NULL,
        .spi_transfer = failing_transfer,
        .pin_write = pin_write_stub,
        .pin_read = pin_read_stub,
        .delay_us = delay_stub,
    };
    uint8_t tx[2] = {0x12u, 0x34u};
    uint8_t rx[2] = {0u, 0u};

    assert(ads1299_mcu_port_init(&ctx, &hal) == 0);
    assert(ads1299_mcu_spi_transfer(&ctx, tx, rx, sizeof(tx)) == -37);
    assert(ads1299_mcu_spi_write(&ctx, tx, sizeof(tx)) == -37);
    assert(ads1299_mcu_spi_read(&ctx, rx, sizeof(rx)) == -37);

    return 0;
}
