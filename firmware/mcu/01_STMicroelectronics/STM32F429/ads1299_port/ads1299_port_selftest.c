#include <stdio.h>
#include <string.h>

#include "ads1299_spi.h"
#include "ads1299_gpio.h"
#include "ads1299_drdy.h"

typedef struct {
    ads1299_platform_pin_t last_pin;
    int last_level;
    int pin_write_count;
    int drdy_level;
    uint32_t delayed_us;
    int spi_call_count;
    uint8_t last_tx[8];
    size_t last_len;
} fake_hal_state_t;

static int fake_spi_transfer(void *user, const uint8_t *tx, uint8_t *rx, size_t len) {
    fake_hal_state_t *s = (fake_hal_state_t *)user;
    size_t i;
    if (!s || len > sizeof(s->last_tx)) return -1;
    s->spi_call_count++;
    s->last_len = len;
    for (i = 0; i < len; ++i) {
        const uint8_t v = tx ? tx[i] : (uint8_t)(0xA0u + i);
        s->last_tx[i] = tx ? tx[i] : 0u;
        if (rx) rx[i] = v;
    }
    return 0;
}

static void fake_pin_write(void *user, ads1299_platform_pin_t pin, int level) {
    fake_hal_state_t *s = (fake_hal_state_t *)user;
    if (!s) return;
    s->last_pin = pin;
    s->last_level = level;
    s->pin_write_count++;
}

static int fake_pin_read(void *user, ads1299_platform_pin_t pin) {
    fake_hal_state_t *s = (fake_hal_state_t *)user;
    if (!s || pin != ADS1299_PIN_DRDY) return 1;
    return s->drdy_level;
}

static void fake_delay_us(void *user, uint32_t us) {
    fake_hal_state_t *s = (fake_hal_state_t *)user;
    if (s) s->delayed_us += us;
}

#define CHECK(expr) do { \
    if (!(expr)) { \
        printf("FAIL line %d: %s\n", __LINE__, #expr); \
        return 1; \
    } \
} while (0)

int main(void) {
    fake_hal_state_t state;
    ads1299_platform_hal_t hal;
    ads1299_mcu_port_t ctx;
    ads1299_port_t port;
    uint8_t tx[3] = {0x11u, 0x22u, 0x33u};
    uint8_t rx[3] = {0u, 0u, 0u};

    memset(&state, 0, sizeof(state));
    memset(&hal, 0, sizeof(hal));
    memset(&ctx, 0, sizeof(ctx));

    hal.user = &state;
    hal.spi_transfer = fake_spi_transfer;
    hal.pin_write = fake_pin_write;
    hal.pin_read = fake_pin_read;
    hal.delay_us = fake_delay_us;

    CHECK(ads1299_mcu_port_init(&ctx, &hal) == 0);
    CHECK(ads1299_mcu_port_init(NULL, &hal) != 0);

    port = ads1299_mcu_make_port(&ctx);
    CHECK(port.user == &ctx);
    CHECK(port.spi_transfer != NULL);
    CHECK(port.cs_write != NULL);
    CHECK(port.reset_write != NULL);
    CHECK(port.pwdn_write != NULL);
    CHECK(port.start_write != NULL);
    CHECK(port.drdy_read != NULL);
    CHECK(port.delay_us != NULL);

    CHECK(port.spi_transfer(port.user, tx, rx, sizeof(tx)) == 0);
    CHECK(state.spi_call_count == 1);
    CHECK(state.last_len == sizeof(tx));
    CHECK(memcmp(tx, rx, sizeof(tx)) == 0);
    CHECK(memcmp(tx, state.last_tx, sizeof(tx)) == 0);
    CHECK(ads1299_mcu_spi_transfer(&ctx, NULL, NULL, 1u) != 0);
    CHECK(ads1299_mcu_spi_transfer(&ctx, tx, rx, 0u) != 0);

    port.cs_write(port.user, 0);
    CHECK(state.last_pin == ADS1299_PIN_CS && state.last_level == 0);
    port.cs_write(port.user, 1);
    CHECK(state.last_pin == ADS1299_PIN_CS && state.last_level == 1);
    port.reset_write(port.user, 0);
    CHECK(state.last_pin == ADS1299_PIN_RESET && state.last_level == 0);
    port.pwdn_write(port.user, 1);
    CHECK(state.last_pin == ADS1299_PIN_PWDN && state.last_level == 1);
    port.start_write(port.user, 1);
    CHECK(state.last_pin == ADS1299_PIN_START && state.last_level == 1);

    state.drdy_level = 0;
    CHECK(ads1299_mcu_data_ready(&ctx) == 1);
    CHECK(port.drdy_read(port.user) == 0);
    state.drdy_level = 1;
    CHECK(ads1299_mcu_data_ready(&ctx) == 0);
    CHECK(port.drdy_read(port.user) == 1);

    port.delay_us(port.user, 25u);
    ads1299_mcu_delay_ms(&ctx, 2u);
    CHECK(state.delayed_us == 2025u);

    CHECK(state.pin_write_count >= 5);
    puts("PASS STM32F429 ads1299_port host self-test");
    return 0;
}
