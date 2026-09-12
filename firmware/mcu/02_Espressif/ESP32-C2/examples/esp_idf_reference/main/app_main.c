#include <inttypes.h>

#include "ads1299.h"
#include "ads1299_espidf_hal.h"
#include "esp_log.h"

static const char *TAG = "ads1299-c2";

void app_main(void)
{
    ads1299_espidf_hal_ctx_t idf_hal;
    ads1299_platform_hal_t hal;
    ads1299_mcu_port_t mcu;
    ads1299_t device;
    ads1299_device_id_t identity;
    ads1299_frame_t frame;

    int rc = ads1299_espidf_hal_init(&idf_hal, &hal);
    if (rc != 0) {
        ESP_LOGE(TAG, "ESP-IDF HAL init failed: %d", rc);
        return;
    }
    if (ads1299_mcu_port_init(&mcu, &hal) != 0) {
        ESP_LOGE(TAG, "ADS1299 portable port init failed");
        return;
    }

    ads1299_port_t port = ads1299_mcu_make_port(&mcu);
    if (ads1299_init(&device, &port) != ADS1299_OK) {
        ESP_LOGE(TAG, "core init failed");
        return;
    }
    if (ads1299_hardware_reset(&device) != ADS1299_OK ||
        ads1299_sdatac(&device) != ADS1299_OK) {
        ESP_LOGE(TAG, "reset/SDATAC failed");
        return;
    }

    if (ads1299_read_device_id(&device, &identity) != ADS1299_OK) {
        ESP_LOGE(TAG, "ID read failed: check power, wiring and SPI mode 1");
        return;
    }
    if (!identity.is_ads1299_family) {
        ESP_LOGE(TAG, "unexpected ID 0x%02x: ADS1299 family not detected", identity.raw);
        return;
    }
    ESP_LOGI(TAG, "probe OK: ADS1299-family ID=0x%02x", identity.raw);

    if (ads1299_configure_internal_test(&device,
                                        ADS1299_GAIN_24,
                                        0,
                                        ADS1299_TEST_FREQ_FCLK_DIV_2_21) != ADS1299_OK) {
        ESP_LOGE(TAG, "internal-test configuration failed");
        return;
    }
    if (ads1299_rdatac(&device) != ADS1299_OK ||
        ads1299_start(&device) != ADS1299_OK) {
        ESP_LOGE(TAG, "continuous start failed");
        return;
    }

    ESP_LOGI(TAG, "internal-test running; waiting for 8 DRDY frames");
    unsigned captured = 0;
    uint32_t spins = 0;
    while (captured < 8u) {
        if (port.drdy_read(port.user) == 0) {
            if (ads1299_read_frame_continuous(&device, &frame) != ADS1299_OK) {
                ESP_LOGE(TAG, "frame read failed");
                break;
            }
            ESP_LOGI(TAG,
                     "frame %u status=%02x%02x%02x ch1=%" PRId32,
                     captured,
                     frame.status[0], frame.status[1], frame.status[2],
                     frame.channel[0]);
            ++captured;
            spins = 0;
        } else if (++spins > 5000000u) {
            ESP_LOGE(TAG, "DRDY timeout: check DRDY wiring and START/reset state");
            break;
        }
    }

    (void)ads1299_stop(&device);
    (void)ads1299_sdatac(&device);
    ESP_LOGI(TAG, "reference probe/internal-test sequence finished");
}
