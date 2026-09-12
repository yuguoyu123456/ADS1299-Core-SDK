#include "ads1299_espidf_hal.h"

#include <string.h>

#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_rom_sys.h"
#include "esp32p4_function_ev_ads1299.h"

#define ADS1299_ESP32P4_MAX_TRANSFER 64u

static int gpio_for_pin(ads1299_platform_pin_t pin)
{
    switch (pin) {
    case ADS1299_PIN_CS:    return ADS1299_ESP32P4_PIN_CS;
    case ADS1299_PIN_RESET: return ADS1299_ESP32P4_PIN_RESET;
    case ADS1299_PIN_PWDN:  return ADS1299_ESP32P4_PIN_PWDN;
    case ADS1299_PIN_START: return ADS1299_ESP32P4_PIN_START;
    case ADS1299_PIN_DRDY:  return ADS1299_ESP32P4_PIN_DRDY;
    default:                 return -1;
    }
}

static int idf_spi_transfer(void *user, const uint8_t *tx, uint8_t *rx, size_t len)
{
    ads1299_espidf_hal_ctx_t *ctx = (ads1299_espidf_hal_ctx_t *)user;
    uint8_t zero_tx[ADS1299_ESP32P4_MAX_TRANSFER] = {0};
    spi_transaction_t t;

    if (!ctx || !ctx->spi || len == 0u || len > ADS1299_ESP32P4_MAX_TRANSFER ||
        (!tx && !rx)) {
        return -1;
    }

    memset(&t, 0, sizeof(t));
    t.length = len * 8u;
    t.tx_buffer = tx ? tx : zero_tx;
    t.rx_buffer = rx;
    return spi_device_polling_transmit(ctx->spi, &t) == ESP_OK ? 0 : -1;
}

static void idf_pin_write(void *user, ads1299_platform_pin_t pin, int level)
{
    const int gpio = gpio_for_pin(pin);
    (void)user;
    if (gpio >= 0 && pin != ADS1299_PIN_DRDY) {
        (void)gpio_set_level((gpio_num_t)gpio, level ? 1 : 0);
    }
}

static int idf_pin_read(void *user, ads1299_platform_pin_t pin)
{
    const int gpio = gpio_for_pin(pin);
    (void)user;
    return gpio < 0 ? 1 : gpio_get_level((gpio_num_t)gpio);
}

static void idf_delay_us(void *user, uint32_t us)
{
    (void)user;
    esp_rom_delay_us(us);
}

static int init_control_gpio(void)
{
    const uint64_t outputs =
        (1ULL << ADS1299_ESP32P4_PIN_CS) |
        (1ULL << ADS1299_ESP32P4_PIN_RESET) |
        (1ULL << ADS1299_ESP32P4_PIN_PWDN) |
        (1ULL << ADS1299_ESP32P4_PIN_START);
    gpio_config_t out_cfg = {
        .pin_bit_mask = outputs,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config_t drdy_cfg = {
        .pin_bit_mask = (1ULL << ADS1299_ESP32P4_PIN_DRDY),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE,
    };

    if (gpio_config(&out_cfg) != ESP_OK || gpio_config(&drdy_cfg) != ESP_OK) return -1;

    (void)gpio_set_level((gpio_num_t)ADS1299_ESP32P4_PIN_CS, 1);
    (void)gpio_set_level((gpio_num_t)ADS1299_ESP32P4_PIN_RESET, 1);
    (void)gpio_set_level((gpio_num_t)ADS1299_ESP32P4_PIN_PWDN, 1);
    (void)gpio_set_level((gpio_num_t)ADS1299_ESP32P4_PIN_START, 0);
    return 0;
}

int ads1299_espidf_hal_init(ads1299_espidf_hal_ctx_t *ctx,
                            ads1299_platform_hal_t *hal)
{
    spi_bus_config_t bus_cfg;
    spi_device_interface_config_t dev_cfg;
    esp_err_t err;

    if (!ctx || !hal) return -1;
    memset(ctx, 0, sizeof(*ctx));
    memset(hal, 0, sizeof(*hal));
    if (init_control_gpio() != 0) return -2;

    memset(&bus_cfg, 0, sizeof(bus_cfg));
    bus_cfg.mosi_io_num = ADS1299_ESP32P4_PIN_MOSI;
    bus_cfg.miso_io_num = ADS1299_ESP32P4_PIN_MISO;
    bus_cfg.sclk_io_num = ADS1299_ESP32P4_PIN_SCLK;
    bus_cfg.quadwp_io_num = -1;
    bus_cfg.quadhd_io_num = -1;
    bus_cfg.max_transfer_sz = ADS1299_ESP32P4_MAX_TRANSFER;

    /* Start with polling transfers for deterministic bring-up. The reference
     * application keeps transport work off the DRDY path; DMA may be layered
     * in later without changing the shared ADS1299 core contract. */
    err = spi_bus_initialize(ADS1299_ESP32P4_SPI_HOST, &bus_cfg, SPI_DMA_DISABLED);
    if (err != ESP_OK) return -3;

    memset(&dev_cfg, 0, sizeof(dev_cfg));
    dev_cfg.clock_speed_hz = ADS1299_ESP32P4_SPI_HZ;
    dev_cfg.mode = ADS1299_ESP32P4_SPI_MODE;
    dev_cfg.spics_io_num = -1;
    dev_cfg.queue_size = 1;

    err = spi_bus_add_device(ADS1299_ESP32P4_SPI_HOST, &dev_cfg, &ctx->spi);
    if (err != ESP_OK) {
        (void)spi_bus_free(ADS1299_ESP32P4_SPI_HOST);
        return -4;
    }

    hal->user = ctx;
    hal->spi_transfer = idf_spi_transfer;
    hal->pin_write = idf_pin_write;
    hal->pin_read = idf_pin_read;
    hal->delay_us = idf_delay_us;
    return 0;
}
