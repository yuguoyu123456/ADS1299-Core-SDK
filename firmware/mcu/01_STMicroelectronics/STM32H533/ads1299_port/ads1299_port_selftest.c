#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "ads1299_spi.h"
#include "ads1299_gpio.h"
#include "ads1299_drdy.h"

typedef struct {
    unsigned spi_calls;
    unsigned pin_write_calls;
    unsigned pin_read_calls;
    unsigned delay_calls;
    ads1299_platform_pin_t last_pin;
    int last_level;
    int drdy_level;
    uint32_t last_delay_us;
    uint8_t last_tx[8];
    size_t last_len;
} fake_hal_state_t;

static int fake_spi(void *user, const uint8_t *tx, uint8_t *rx, size_t len) {
    fake_hal_state_t *s = (fake_hal_state_t *)user;
    size_t i;
    s->spi_calls++;
    s->last_len = len;
    for (i = 0; i < len && i < sizeof(s->last_tx); ++i) {
        s->last_tx[i] = tx ? tx[i] : 0u;
        if (rx) rx[i] = tx ? (uint8_t)(tx[i] ^ 0xFFu) : (uint8_t)(0xA0u + i);
    }
    return 0;
}

static void fake_pin_write(void *user, ads1299_platform_pin_t pin, int level) {
    fake_hal_state_t *s = (fake_hal_state_t *)user;
    s->pin_write_calls++;
    s->last_pin = pin;
    s->last_level = level;
}

static int fake_pin_read(void *user, ads1299_platform_pin_t pin) {
    fake_hal_state_t *s = (fake_hal_state_t *)user;
    s->pin_read_calls++;
    s->last_pin = pin;
    return (pin == ADS1299_PIN_DRDY) ? s->drdy_level : 1;
}

static void fake_delay(void *user, uint32_t us) {
    fake_hal_state_t *s = (fake_hal_state_t *)user;
    s->delay_calls++;
    s->last_delay_us = us;
}

static ads1299_mcu_port_t make_context(fake_hal_state_t *state) {
    ads1299_platform_hal_t hal;
    ads1299_mcu_port_t ctx;
    memset(&hal, 0, sizeof(hal));
    memset(&ctx, 0, sizeof(ctx));
    hal.user = state;
    hal.spi_transfer = fake_spi;
    hal.pin_write = fake_pin_write;
    hal.pin_read = fake_pin_read;
    hal.delay_us = fake_delay;
    assert(ads1299_mcu_port_init(&ctx, &hal) == 0);
    return ctx;
}

static void test_init_rejects_incomplete_hal(void) {
    ads1299_mcu_port_t ctx;
    ads1299_platform_hal_t hal;
    memset(&ctx, 0, sizeof(ctx));
    memset(&hal, 0, sizeof(hal));
    assert(ads1299_mcu_port_init(NULL, &hal) != 0);
    assert(ads1299_mcu_port_init(&ctx, NULL) != 0);
    assert(ads1299_mcu_port_init(&ctx, &hal) != 0);
}

static void test_spi_forwarding(void) {
    fake_hal_state_t state;
    ads1299_mcu_port_t ctx;
    uint8_t tx[3] = {0x11u, 0x22u, 0x33u};
    uint8_t rx[3] = {0u};
    memset(&state, 0, sizeof(state));
    ctx = make_context(&state);

    assert(ads1299_mcu_spi_transfer(&ctx, tx, rx, sizeof(tx)) == 0);
    assert(state.spi_calls == 1u);
    assert(state.last_len == sizeof(tx));
    assert(state.last_tx[0] == 0x11u && state.last_tx[2] == 0x33u);
    assert(rx[0] == 0xEEu && rx[2] == 0xCCu);

    assert(ads1299_mcu_spi_transfer(NULL, tx, rx, sizeof(tx)) != 0);
    assert(ads1299_mcu_spi_transfer(&ctx, NULL, NULL, sizeof(tx)) != 0);
    assert(ads1299_mcu_spi_transfer(&ctx, tx, rx, 0u) != 0);
    assert(ads1299_mcu_spi_write(&ctx, NULL, 1u) != 0);
    assert(ads1299_mcu_spi_read(&ctx, NULL, 1u) != 0);
}

static void test_gpio_and_drdy(void) {
    fake_hal_state_t state;
    ads1299_mcu_port_t ctx;
    ads1299_port_t port;
    memset(&state, 0, sizeof(state));
    ctx = make_context(&state);
    port = ads1299_mcu_make_port(&ctx);

    port.cs_write(port.user, 0);
    assert(state.last_pin == ADS1299_PIN_CS && state.last_level == 0);
    port.reset_write(port.user, 1);
    assert(state.last_pin == ADS1299_PIN_RESET && state.last_level == 1);
    port.pwdn_write(port.user, 0);
    assert(state.last_pin == ADS1299_PIN_PWDN && state.last_level == 0);
    port.start_write(port.user, 1);
    assert(state.last_pin == ADS1299_PIN_START && state.last_level == 1);

    state.drdy_level = 0;
    assert(ads1299_mcu_data_ready(&ctx) == 1);
    assert(port.drdy_read(port.user) == 0);
    state.drdy_level = 1;
    assert(ads1299_mcu_data_ready(&ctx) == 0);
    assert(port.drdy_read(port.user) == 1);
}

static void test_delay_forwarding(void) {
    fake_hal_state_t state;
    ads1299_mcu_port_t ctx;
    ads1299_port_t port;
    memset(&state, 0, sizeof(state));
    ctx = make_context(&state);
    port = ads1299_mcu_make_port(&ctx);

    port.delay_us(port.user, 42u);
    assert(state.delay_calls == 1u && state.last_delay_us == 42u);
    ads1299_mcu_delay_ms(&ctx, 2u);
    assert(state.delay_calls == 3u && state.last_delay_us == 1000u);
}

int main(void) {
    test_init_rejects_incomplete_hal();
    test_spi_forwarding();
    test_gpio_and_drdy();
    test_delay_forwarding();
    puts("PASS STM32H533 ads1299_port host self-test");
    return 0;
}
