#include "ads1299_esp32_hal.h"
#include "esp32_devkitc_v4_ads1299.h"
#include "freertos/task.h"

/* Original DevKitC V4 WROOM profile; SPI3 is VSPI on classic ESP32. */
static ads1299_esp32_context_t context;

void board_ads1299_idle(void) { vTaskDelay(1); }

int board_ads1299_hal(ads1299_platform_hal_t *hal)
{
    const gpio_num_t pins[5] = {
        ADS1299_BOARD_PIN_CS,
        ADS1299_BOARD_PIN_RESET,
        ADS1299_BOARD_PIN_PWDN,
        ADS1299_BOARD_PIN_START,
        ADS1299_BOARD_PIN_DRDY,
    };
    const gpio_config_t out = {
        .pin_bit_mask = (1ULL << ADS1299_BOARD_PIN_CS) |
                        (1ULL << ADS1299_BOARD_PIN_RESET) |
                        (1ULL << ADS1299_BOARD_PIN_PWDN) |
                        (1ULL << ADS1299_BOARD_PIN_START),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    const gpio_config_t in = {
        .pin_bit_mask = 1ULL << ADS1299_BOARD_PIN_DRDY,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    if (gpio_config(&out) != ESP_OK || gpio_config(&in) != ESP_OK) return -1;
    if (gpio_set_level(ADS1299_BOARD_PIN_CS, 1) != ESP_OK) return -1;
    if (gpio_set_level(ADS1299_BOARD_PIN_RESET, 1) != ESP_OK) return -1;
    if (gpio_set_level(ADS1299_BOARD_PIN_PWDN, 1) != ESP_OK) return -1;
    if (gpio_set_level(ADS1299_BOARD_PIN_START, 0) != ESP_OK) return -1;

    const spi_bus_config_t bus = {
        .mosi_io_num = ADS1299_BOARD_PIN_MOSI,
        .miso_io_num = ADS1299_BOARD_PIN_MISO,
        .sclk_io_num = ADS1299_BOARD_PIN_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 32,
    };
    const spi_device_interface_config_t dev = {
        .clock_speed_hz = ADS1299_BOARD_SPI_HZ,
        .mode = 1,
        .spics_io_num = -1,
        .queue_size = 1,
    };
    if (spi_bus_initialize(ADS1299_BOARD_SPI_HOST, &bus, SPI_DMA_DISABLED) != ESP_OK) return -1;
    if (spi_bus_add_device(ADS1299_BOARD_SPI_HOST, &dev, &context.spi) != ESP_OK) {
        (void)spi_bus_free(ADS1299_BOARD_SPI_HOST);
        return -1;
    }
    for (unsigned i = 0; i < 5; ++i) context.pins[i] = pins[i];
    context.timeout_ticks = pdMS_TO_TICKS(100);
    return ads1299_esp32_make_hal(&context, hal);
}
