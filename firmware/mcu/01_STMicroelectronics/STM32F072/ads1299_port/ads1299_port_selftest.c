#include <stdio.h>
#include <string.h>

#include "ads1299_spi.h"
#include "ads1299_gpio.h"
#include "ads1299_drdy.h"

typedef struct {
    int pins[5];
    uint32_t delay_total;
    size_t spi_len;
    uint8_t tx_seen[8];
    int drdy_level;
} fake_hal_t;

static int fake_spi(void *user, const uint8_t *tx, uint8_t *rx, size_t len) {
    fake_hal_t *fake = (fake_hal_t *)user;
    size_t i;

    fake->spi_len = len;
    for (i = 0; i < len && i < sizeof fake->tx_seen; ++i) {
        fake->tx_seen[i] = tx ? tx[i] : 0u;
    }
    if (rx) {
        for (i = 0; i < len; ++i) {
            rx[i] = (uint8_t)(0xA0u + i);
        }
    }
    return 0;
}

static void fake_pin_write(void *user, ads1299_platform_pin_t pin, int level) {
    fake_hal_t *fake = (fake_hal_t *)user;
    if ((unsigned)pin < 5u) {
        fake->pins[pin] = level;
    }
}

static int fake_pin_read(void *user, ads1299_platform_pin_t pin) {
    fake_hal_t *fake = (fake_hal_t *)user;
    return pin == ADS1299_PIN_DRDY ? fake->drdy_level : fake->pins[pin];
}

static void fake_delay_us(void *user, uint32_t us) {
    ((fake_hal_t *)user)->delay_total += us;
}

static int check(int condition, const char *message) {
    if (!condition) {
        fprintf(stderr, "FAIL: %s\n", message);
        return 1;
    }
    return 0;
}

int main(void) {
    fake_hal_t fake;
    ads1299_platform_hal_t hal;
    ads1299_mcu_port_t ctx;
    ads1299_port_t port;
    uint8_t tx[3] = {0x11u, 0x22u, 0x33u};
    uint8_t rx[3] = {0};
    int failed = 0;

    memset(&fake, 0, sizeof fake);
    hal.user = &fake;
    hal.spi_transfer = fake_spi;
    hal.pin_write = fake_pin_write;
    hal.pin_read = fake_pin_read;
    hal.delay_us = fake_delay_us;

    failed |= check(ads1299_mcu_port_init(&ctx, &hal) == 0, "port init");
    port = ads1299_mcu_make_port(&ctx);

    failed |= check(port.spi_transfer(port.user, tx, rx, 3u) == 0, "SPI transfer");
    failed |= check(fake.spi_len == 3u && fake.tx_seen[0] == 0x11u && rx[2] == 0xA2u,
                    "SPI data forwarding");

    port.cs_write(port.user, 0);
    port.reset_write(port.user, 0);
    port.pwdn_write(port.user, 1);
    port.start_write(port.user, 1);
    failed |= check(fake.pins[ADS1299_PIN_CS] == 0 &&
                    fake.pins[ADS1299_PIN_RESET] == 0 &&
                    fake.pins[ADS1299_PIN_PWDN] == 1 &&
                    fake.pins[ADS1299_PIN_START] == 1,
                    "GPIO routing");

    fake.drdy_level = 0;
    failed |= check(ads1299_mcu_data_ready(&ctx) == 1 && port.drdy_read(port.user) == 0,
                    "DRDY active-low ready");
    fake.drdy_level = 1;
    failed |= check(ads1299_mcu_data_ready(&ctx) == 0 && port.drdy_read(port.user) == 1,
                    "DRDY inactive-high");

    port.delay_us(port.user, 25u);
    ads1299_mcu_delay_ms(&ctx, 2u);
    failed |= check(fake.delay_total == 2025u, "delay routing");

    failed |= check(ads1299_mcu_port_init(NULL, &hal) != 0, "reject null context");
    failed |= check(ads1299_mcu_spi_transfer(&ctx, NULL, NULL, 1u) != 0,
                    "reject empty SPI buffers");

    if (failed) {
        return 1;
    }

    puts("PASS STM32F072 ads1299_port host self-test");
    return 0;
}
