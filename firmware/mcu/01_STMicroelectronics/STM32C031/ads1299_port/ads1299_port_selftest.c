#include <stdio.h>
#include <string.h>

#include "ads1299_spi.h"
#include "ads1299_gpio.h"
#include "ads1299_drdy.h"

typedef struct {
    int pin_level[5];
    unsigned writes[5];
    uint32_t delayed_us;
    unsigned spi_calls;
    uint8_t last_tx[8];
    size_t last_len;
} fake_hal_t;

static int fake_spi_transfer(void *user,
                             const uint8_t *tx,
                             uint8_t *rx,
                             size_t len) {
    fake_hal_t *fake = (fake_hal_t *)user;
    size_t i;

    fake->spi_calls++;
    fake->last_len = len;
    for (i = 0; i < len && i < sizeof(fake->last_tx); ++i) {
        fake->last_tx[i] = tx ? tx[i] : 0u;
        if (rx) rx[i] = (uint8_t)(0xA0u + i);
    }
    return 0;
}

static void fake_pin_write(void *user,
                           ads1299_platform_pin_t pin,
                           int level) {
    fake_hal_t *fake = (fake_hal_t *)user;
    fake->pin_level[pin] = level;
    fake->writes[pin]++;
}

static int fake_pin_read(void *user, ads1299_platform_pin_t pin) {
    fake_hal_t *fake = (fake_hal_t *)user;
    return fake->pin_level[pin];
}

static void fake_delay_us(void *user, uint32_t us) {
    fake_hal_t *fake = (fake_hal_t *)user;
    fake->delayed_us += us;
}

#define CHECK(expr)                                                        \
    do {                                                                   \
        if (!(expr)) {                                                     \
            printf("FAIL:%s:%d: %s\n", __FILE__, __LINE__, #expr);       \
            return 1;                                                      \
        }                                                                  \
    } while (0)

int main(void) {
    fake_hal_t fake;
    ads1299_mcu_port_t ctx;
    ads1299_platform_hal_t hal;
    ads1299_port_t port;
    uint8_t tx[3] = {0x11u, 0x22u, 0x33u};
    uint8_t rx[3] = {0u};

    memset(&fake, 0, sizeof(fake));
    memset(&ctx, 0, sizeof(ctx));
    memset(&hal, 0, sizeof(hal));

    /* Required callbacks must be present before a platform port is valid. */
    CHECK(ads1299_mcu_port_init(NULL, &hal) == -1);
    CHECK(ads1299_mcu_port_init(&ctx, &hal) == -1);

    hal.user = &fake;
    hal.spi_transfer = fake_spi_transfer;
    hal.pin_write = fake_pin_write;
    hal.pin_read = fake_pin_read;
    hal.delay_us = fake_delay_us;
    CHECK(ads1299_mcu_port_init(&ctx, &hal) == 0);

    port = ads1299_mcu_make_port(&ctx);
    CHECK(port.user == &ctx);
    CHECK(port.spi_transfer != NULL);
    CHECK(port.cs_write != NULL);
    CHECK(port.reset_write != NULL);
    CHECK(port.pwdn_write != NULL);
    CHECK(port.start_write != NULL);
    CHECK(port.drdy_read != NULL);
    CHECK(port.delay_us != NULL);

    /* SPI data must pass through the portable ADS1299 port unchanged. */
    CHECK(port.spi_transfer(port.user, tx, rx, sizeof(tx)) == 0);
    CHECK(fake.spi_calls == 1u);
    CHECK(fake.last_len == sizeof(tx));
    CHECK(memcmp(fake.last_tx, tx, sizeof(tx)) == 0);
    CHECK(rx[0] == 0xA0u && rx[1] == 0xA1u && rx[2] == 0xA2u);

    /* Every ADS1299 control signal must route to the correct board pin ID. */
    port.cs_write(port.user, 0);
    CHECK(fake.pin_level[ADS1299_PIN_CS] == 0);
    CHECK(fake.writes[ADS1299_PIN_CS] == 1u);

    port.reset_write(port.user, 1);
    port.pwdn_write(port.user, 1);
    port.start_write(port.user, 1);
    CHECK(fake.pin_level[ADS1299_PIN_RESET] == 1);
    CHECK(fake.pin_level[ADS1299_PIN_PWDN] == 1);
    CHECK(fake.pin_level[ADS1299_PIN_START] == 1);

    /* ADS1299 DRDY is active low: physical low means data ready. */
    fake.pin_level[ADS1299_PIN_DRDY] = 1;
    CHECK(ads1299_mcu_data_ready(&ctx) == 0);
    CHECK(port.drdy_read(port.user) == 1);

    fake.pin_level[ADS1299_PIN_DRDY] = 0;
    CHECK(ads1299_mcu_data_ready(&ctx) == 1);
    CHECK(port.drdy_read(port.user) == 0);

    /* Delay forwarding and basic invalid-transfer guards. */
    port.delay_us(port.user, 25u);
    ads1299_mcu_delay_ms(&ctx, 2u);
    CHECK(fake.delayed_us == 2025u);
    CHECK(ads1299_mcu_spi_transfer(&ctx, NULL, NULL, 1u) == -1);
    CHECK(ads1299_mcu_spi_transfer(&ctx, tx, NULL, 0u) == -1);

    puts("PASS STM32C031 ads1299_port host self-test");
    return 0;
}
