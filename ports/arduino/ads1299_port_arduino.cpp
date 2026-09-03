#include "ads1299_port_arduino.h"

static int ard_spi_transfer(void *user, const uint8_t *tx, uint8_t *rx, size_t len) {
    auto *ctx = static_cast<ads1299_arduino_ctx_t *>(user);
    if (!ctx || !ctx->spi) return -1;
    const uint32_t hz = ctx->spi_clock_hz ? ctx->spi_clock_hz : 1000000u;
    ctx->spi->beginTransaction(SPISettings(hz, MSBFIRST, SPI_MODE1));
    for (size_t i = 0; i < len; ++i) {
        uint8_t r = ctx->spi->transfer(tx ? tx[i] : 0x00u);
        if (rx) rx[i] = r;
    }
    ctx->spi->endTransaction();
    return 0;
}

static void ard_cs(void *user, int level) {
    auto *ctx = static_cast<ads1299_arduino_ctx_t *>(user);
    digitalWrite(ctx->cs_pin, level ? HIGH : LOW);
}

static void ard_reset(void *user, int level) {
    auto *ctx = static_cast<ads1299_arduino_ctx_t *>(user);
    if (ctx->reset_pin >= 0) digitalWrite(ctx->reset_pin, level ? HIGH : LOW);
}

static void ard_start(void *user, int level) {
    auto *ctx = static_cast<ads1299_arduino_ctx_t *>(user);
    if (ctx->start_pin >= 0) digitalWrite(ctx->start_pin, level ? HIGH : LOW);
}

static int ard_drdy(void *user) {
    auto *ctx = static_cast<ads1299_arduino_ctx_t *>(user);
    return ctx->drdy_pin >= 0 ? digitalRead(ctx->drdy_pin) : HIGH;
}

static void ard_delay(void *user, uint32_t us) {
    (void)user;
    delayMicroseconds(us);
}

ads1299_port_t ads1299_arduino_make_port(ads1299_arduino_ctx_t *ctx) {
    pinMode(ctx->cs_pin, OUTPUT);
    digitalWrite(ctx->cs_pin, HIGH);
    if (ctx->reset_pin >= 0) pinMode(ctx->reset_pin, OUTPUT);
    if (ctx->start_pin >= 0) pinMode(ctx->start_pin, OUTPUT);
    if (ctx->drdy_pin >= 0) pinMode(ctx->drdy_pin, INPUT);

    ads1299_port_t p = {
        .user = ctx,
        .spi_transfer = ard_spi_transfer,
        .cs_write = ard_cs,
        .reset_write = ard_reset,
        .start_write = ard_start,
        .drdy_read = ard_drdy,
        .delay_us = ard_delay,
    };
    return p;
}
