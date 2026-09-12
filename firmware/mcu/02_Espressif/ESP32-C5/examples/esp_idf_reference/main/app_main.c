#include <inttypes.h>
#include <stdbool.h>

#include "ads1299.h"
#include "ads1299_espidf_hal.h"
#include "esp_log.h"

static const char *TAG = "ads1299-c5";

#define DIAG_FRAMES 8u
#define DRDY_TIMEOUT_US 500000u
#define DRDY_POLL_US 100u

static bool start_continuous(ads1299_t *dev, const char *phase)
{
    ads1299_status_t a = ads1299_rdatac(dev);
    ads1299_status_t b = ads1299_start(dev);
    if (a != ADS1299_OK || b != ADS1299_OK) {
        ESP_LOGE(TAG, "%s start failed: rdatac=%d start=%d", phase, (int)a, (int)b);
        return false;
    }
    return true;
}

static bool stop_continuous(ads1299_t *dev, const char *phase)
{
    ads1299_status_t a = ads1299_stop(dev);
    ads1299_status_t b = ads1299_sdatac(dev);
    if (a != ADS1299_OK || b != ADS1299_OK) {
        ESP_LOGE(TAG, "%s stop failed: stop=%d sdatac=%d", phase, (int)a, (int)b);
        return false;
    }
    return true;
}

static bool capture_frames(ads1299_t *dev, const char *phase)
{
    ads1299_frame_t frame;
    for (uint32_t i = 0; i < DIAG_FRAMES; ++i) {
        ads1299_status_t rc = ads1299_wait_drdy(dev, DRDY_TIMEOUT_US, DRDY_POLL_US);
        if (rc != ADS1299_OK) {
            ESP_LOGE(TAG, "%s DRDY timeout at frame %" PRIu32, phase, i);
            return false;
        }
        rc = ads1299_read_frame_continuous(dev, &frame);
        if (rc != ADS1299_OK) {
            ESP_LOGE(TAG, "%s frame read failed at frame %" PRIu32, phase, i);
            return false;
        }
        ESP_LOGI(TAG, "%s frame=%" PRIu32 " status=%02x%02x%02x ch1=%" PRId32,
                 phase, i, frame.status[0], frame.status[1], frame.status[2], frame.channel[0]);
    }
    return true;
}

static bool run_internal_test(ads1299_t *dev)
{
    if (ads1299_configure_internal_test(dev, ADS1299_GAIN_24, 0,
                                        ADS1299_TEST_FREQ_FCLK_DIV_2_21) != ADS1299_OK) {
        ESP_LOGE(TAG, "internal-test configuration failed");
        return false;
    }
    if (!start_continuous(dev, "internal-test")) return false;
    bool ok = capture_frames(dev, "internal-test");
    return stop_continuous(dev, "internal-test") && ok;
}

static bool run_input_short(ads1299_t *dev)
{
    if (ads1299_configure_input_short_test(dev, ADS1299_GAIN_24) != ADS1299_OK) {
        ESP_LOGE(TAG, "input-short configuration failed");
        return false;
    }
    if (!start_continuous(dev, "input-short")) return false;
    bool ok = capture_frames(dev, "input-short");
    return stop_continuous(dev, "input-short") && ok;
}

static bool configure_eeg250(ads1299_t *dev, uint8_t channels)
{
    if (ads1299_set_data_rate(dev, ADS1299_DR_250SPS) != ADS1299_OK) return false;
    if (ads1299_set_srb1(dev, 0) != ADS1299_OK) return false;
    for (uint8_t ch = 1u; ch <= channels; ++ch) {
        if (ads1299_set_channel(dev, ch, ADS1299_GAIN_24,
                                ADS1299_MUX_NORMAL, 0, 0) != ADS1299_OK) {
            return false;
        }
    }
    return true;
}

void app_main(void)
{
    ads1299_espidf_hal_ctx_t idf_hal;
    ads1299_platform_hal_t hal;
    ads1299_mcu_port_t mcu;
    ads1299_port_t port;
    ads1299_t dev;
    ads1299_device_id_t id;

    int rc = ads1299_espidf_hal_init(&idf_hal, &hal);
    if (rc != 0) {
        ESP_LOGE(TAG, "HAL init failed: %d", rc);
        return;
    }
    if (ads1299_mcu_port_init(&mcu, &hal) != 0) {
        ESP_LOGE(TAG, "portable MCU port init failed");
        return;
    }
    port = ads1299_mcu_make_port(&mcu);
    if (ads1299_init(&dev, &port) != ADS1299_OK) {
        ESP_LOGE(TAG, "shared core init failed");
        return;
    }
    if (ads1299_hardware_reset(&dev) != ADS1299_OK || ads1299_sdatac(&dev) != ADS1299_OK) {
        ESP_LOGE(TAG, "RESET/SDATAC failed");
        return;
    }
    if (ads1299_read_device_id(&dev, &id) != ADS1299_OK || !id.is_ads1299_family) {
        ESP_LOGE(TAG, "probe failed: check ADS1299 power, wiring and SPI Mode 1");
        return;
    }

    ESP_LOGI(TAG, "probe OK: ADS1299-family ID=0x%02x channels=%u",
             id.raw, (unsigned)id.channel_count);

    if (!run_internal_test(&dev)) return;
    if (!run_input_short(&dev)) return;
    if (!configure_eeg250(&dev, id.channel_count)) {
        ESP_LOGE(TAG, "250-SPS EEG configuration failed");
        return;
    }
    if (!start_continuous(&dev, "EEG250")) return;
    if (!capture_frames(&dev, "EEG250")) {
        (void)stop_continuous(&dev, "EEG250");
        return;
    }
    if (!stop_continuous(&dev, "EEG250")) return;

    ESP_LOGI(TAG, "beginner diagnostic flow complete: probe -> internal-test -> input-short -> EEG250");
    ESP_LOGI(TAG, "continuous wireless streaming is not enabled yet; add a bounded queue before Wi-Fi/BLE transport");
}
