#include "ads1299_esp_idf_hal.h"

#include "esp_rom_sys.h"

#include <string.h>

#define ADS1299_ESP_IDF_CHUNK_BYTES 32u

static int esp_spi_transfer(void *user, const uint8_t *tx, uint8_t *rx,
                            size_t len) {
    ads1299_esp_idf_ctx_t *ctx = (ads1299_esp_idf_ctx_t *)user;
    size_t offset = 0u;

    if (!ctx || !ctx->spi || len == 0u || (!tx && !rx)) return -1;

    while (offset < len) {
        uint8_t zeros[ADS1299_ESP_IDF_CHUNK_BYTES] = {0};
        uint8_t discard[ADS1299_ESP_IDF_CHUNK_BYTES];
        const size_t remaining = len - offset;
        const size_t chunk = remaining < sizeof(zeros) ? remaining : sizeof(zeros);
        spi_transaction_t transaction;

        memset(&transaction, 0, sizeof(transaction));
        transaction.length = chunk * 8u;
        transaction.tx_buffer = tx ? tx + offset : zeros;
        transaction.rx_buffer = rx ? rx + offset : discard;
        if (spi_device_polling_transmit(ctx->spi, &transaction) != ESP_OK) {
            return -1;
        }
        offset += chunk;
    }
    return 0;
}

static gpio_num_t pin_number(const ads1299_esp_idf_ctx_t *ctx,
                             ads1299_platform_pin_t pin) {
    switch (pin) {
        case ADS1299_PIN_CS: return ctx->cs_pin;
        case ADS1299_PIN_RESET: return ctx->reset_pin;
        case ADS1299_PIN_PWDN: return ctx->pwdn_pin;
        case ADS1299_PIN_START: return ctx->start_pin;
        case ADS1299_PIN_DRDY: return ctx->drdy_pin;
        default: return GPIO_NUM_NC;
    }
}

static void esp_pin_write(void *user, ads1299_platform_pin_t pin, int level) {
    ads1299_esp_idf_ctx_t *ctx = (ads1299_esp_idf_ctx_t *)user;
    gpio_num_t gpio;
    if (!ctx) return;
    gpio = pin_number(ctx, pin);
    if (gpio != GPIO_NUM_NC) (void)gpio_set_level(gpio, level ? 1u : 0u);
}

static int esp_pin_read(void *user, ads1299_platform_pin_t pin) {
    ads1299_esp_idf_ctx_t *ctx = (ads1299_esp_idf_ctx_t *)user;
    gpio_num_t gpio;
    if (!ctx) return 1;
    gpio = pin_number(ctx, pin);
    return gpio == GPIO_NUM_NC ? 1 : gpio_get_level(gpio);
}

static void esp_delay_us(void *user, uint32_t us) {
    (void)user;
    esp_rom_delay_us(us);
}

int ads1299_esp_idf_make_hal(ads1299_platform_hal_t *hal,
                             ads1299_esp_idf_ctx_t *ctx) {
    if (!hal || !ctx || !ctx->spi) return -1;
    *hal = (ads1299_platform_hal_t){
        .user = ctx,
        .spi_transfer = esp_spi_transfer,
        .pin_write = esp_pin_write,
        .pin_read = esp_pin_read,
        .delay_us = esp_delay_us,
    };
    return 0;
}
