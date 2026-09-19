#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include "ads1299_spi.h"

static int stub_spi(void *user, const uint8_t *tx, uint8_t *rx, size_t len)
{
    (void)user;
    (void)tx;
    (void)rx;
    return len > 0U ? 0 : -1;
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

int main(void)
{
    ads1299_mcu_port_t port;
    ads1299_platform_hal_t hal = {
        .user = NULL,
        .spi_transfer = stub_spi,
        .pin_write = stub_write,
        .pin_read = stub_read,
        .delay_us = stub_delay,
    };

    assert(ads1299_mcu_port_init(NULL, &hal) != 0);
    assert(ads1299_mcu_port_init(&port, NULL) != 0);

    ads1299_platform_hal_t missing = hal;
    missing.spi_transfer = NULL;
    assert(ads1299_mcu_port_init(&port, &missing) != 0);

    missing = hal;
    missing.pin_write = NULL;
    assert(ads1299_mcu_port_init(&port, &missing) != 0);

    missing = hal;
    missing.pin_read = NULL;
    assert(ads1299_mcu_port_init(&port, &missing) != 0);

    missing = hal;
    missing.delay_us = NULL;
    assert(ads1299_mcu_port_init(&port, &missing) != 0);

    assert(ads1299_mcu_port_init(&port, &hal) == 0);

    uint8_t byte = 0U;
    assert(ads1299_mcu_spi_transfer(NULL, &byte, &byte, 1U) != 0);
    assert(ads1299_mcu_spi_transfer(&port, NULL, NULL, 1U) != 0);
    assert(ads1299_mcu_spi_transfer(&port, &byte, &byte, 0U) != 0);
    assert(ads1299_mcu_spi_write(&port, NULL, 1U) != 0);
    assert(ads1299_mcu_spi_read(&port, NULL, 1U) != 0);

    assert(ads1299_mcu_spi_transfer(&port, &byte, &byte, 1U) == 0);
    assert(ads1299_mcu_spi_write(&port, &byte, 1U) == 0);
    assert(ads1299_mcu_spi_read(&port, &byte, 1U) == 0);

    return 0;
}
