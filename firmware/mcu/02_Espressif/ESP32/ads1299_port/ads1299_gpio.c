#include "ads1299_gpio.h"
#include "ads1299_spi.h"

static void write_pin(void *user, ads1299_platform_pin_t pin, int level) {
    ads1299_mcu_port_t *ctx = (ads1299_mcu_port_t *)user;
    if (ctx && ctx->hal.pin_write) {
        ctx->hal.pin_write(ctx->hal.user, pin, level ? 1 : 0);
    }
}

void ads1299_port_cs_write(void *user, int level) {
    write_pin(user, ADS1299_PIN_CS, level);
}
void ads1299_port_reset_write(void *user, int level) {
    write_pin(user, ADS1299_PIN_RESET, level);
}
void ads1299_port_pwdn_write(void *user, int level) {
    write_pin(user, ADS1299_PIN_PWDN, level);
}
void ads1299_port_start_write(void *user, int level) {
    write_pin(user, ADS1299_PIN_START, level);
}
void ads1299_mcu_cs_low(ads1299_mcu_port_t *ctx) { write_pin(ctx, ADS1299_PIN_CS, 0); }
void ads1299_mcu_cs_high(ads1299_mcu_port_t *ctx) { write_pin(ctx, ADS1299_PIN_CS, 1); }
void ads1299_mcu_reset(ads1299_mcu_port_t *ctx, int level) {
    write_pin(ctx, ADS1299_PIN_RESET, level);
}
void ads1299_mcu_start(ads1299_mcu_port_t *ctx, int level) {
    write_pin(ctx, ADS1299_PIN_START, level);
}
void ads1299_mcu_delay_us(ads1299_mcu_port_t *ctx, uint32_t us) {
    if (ctx && ctx->hal.delay_us) ctx->hal.delay_us(ctx->hal.user, us);
}
void ads1299_mcu_delay_ms(ads1299_mcu_port_t *ctx, uint32_t ms) {
    while (ms-- != 0u) ads1299_mcu_delay_us(ctx, 1000u);
}
