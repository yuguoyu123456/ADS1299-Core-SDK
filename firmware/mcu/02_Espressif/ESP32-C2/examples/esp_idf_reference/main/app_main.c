#include <inttypes.h>

#include "ads1299.h"
#include "ads1299_espidf_hal.h"
#include "esp32c2_frame_queue.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "ads1299-c2";

static ads1299_espidf_hal_ctx_t g_idf_hal;
static ads1299_platform_hal_t g_hal;
static ads1299_mcu_port_t g_mcu;
static ads1299_port_t g_port;
static ads1299_t g_device;
static esp32c2_ads1299_frame_queue_t g_queue;
static portMUX_TYPE g_queue_lock = portMUX_INITIALIZER_UNLOCKED;
static uint32_t g_sequence;

#define ADS1299_DIAGNOSTIC_FRAMES 8u
#define ADS1299_DRDY_TIMEOUT_US 500000u
#define ADS1299_DRDY_POLL_US 100u

static bool stop_continuous(const char *phase)
{
    ads1299_status_t rc_stop = ads1299_stop(&g_device);
    ads1299_status_t rc_sdatac = ads1299_sdatac(&g_device);
    if (rc_stop != ADS1299_OK || rc_sdatac != ADS1299_OK) {
        ESP_LOGE(TAG, "%s stop/SDATAC failed: stop=%d sdatac=%d",
                 phase, (int)rc_stop, (int)rc_sdatac);
        return false;
    }
    return true;
}

static bool start_continuous(const char *phase)
{
    ads1299_status_t rc_rdatac = ads1299_rdatac(&g_device);
    ads1299_status_t rc_start = ads1299_start(&g_device);
    if (rc_rdatac != ADS1299_OK || rc_start != ADS1299_OK) {
        ESP_LOGE(TAG, "%s RDATAC/START failed: rdatac=%d start=%d",
                 phase, (int)rc_rdatac, (int)rc_start);
        return false;
    }
    return true;
}

static bool capture_diagnostic_frames(const char *phase, uint32_t frame_count)
{
    ads1299_frame_t frame;

    for (uint32_t i = 0; i < frame_count; ++i) {
        ads1299_status_t rc = ads1299_wait_drdy(
            &g_device, ADS1299_DRDY_TIMEOUT_US, ADS1299_DRDY_POLL_US);
        if (rc != ADS1299_OK) {
            ESP_LOGE(TAG, "%s DRDY timeout/failure at frame %" PRIu32 ": %d",
                     phase, i, (int)rc);
            return false;
        }

        rc = ads1299_read_frame_continuous(&g_device, &frame);
        if (rc != ADS1299_OK) {
            ESP_LOGE(TAG, "%s frame read failed at frame %" PRIu32 ": %d",
                     phase, i, (int)rc);
            return false;
        }

        ESP_LOGI(TAG,
                 "%s frame=%" PRIu32 " status=%02x%02x%02x ch1=%" PRId32,
                 phase,
                 i,
                 frame.status[0],
                 frame.status[1],
                 frame.status[2],
                 frame.channel[0]);
    }

    return true;
}

static bool run_internal_test(void)
{
    if (ads1299_configure_internal_test(&g_device,
                                        ADS1299_GAIN_24,
                                        0,
                                        ADS1299_TEST_FREQ_FCLK_DIV_2_21) != ADS1299_OK) {
        ESP_LOGE(TAG, "internal-test configuration failed");
        return false;
    }
    if (!start_continuous("internal-test")) {
        return false;
    }

    ESP_LOGI(TAG, "internal-test: capturing %u frames",
             (unsigned)ADS1299_DIAGNOSTIC_FRAMES);
    bool ok = capture_diagnostic_frames("internal-test", ADS1299_DIAGNOSTIC_FRAMES);
    return stop_continuous("internal-test") && ok;
}

static bool run_input_short_test(void)
{
    if (ads1299_configure_input_short_test(&g_device, ADS1299_GAIN_24) != ADS1299_OK) {
        ESP_LOGE(TAG, "input-short configuration failed");
        return false;
    }
    if (!start_continuous("input-short")) {
        return false;
    }

    ESP_LOGI(TAG, "input-short: capturing %u frames",
             (unsigned)ADS1299_DIAGNOSTIC_FRAMES);
    bool ok = capture_diagnostic_frames("input-short", ADS1299_DIAGNOSTIC_FRAMES);
    return stop_continuous("input-short") && ok;
}

static bool configure_eeg_250(uint8_t channel_count)
{
    if (ads1299_set_data_rate(&g_device, ADS1299_DR_250SPS) != ADS1299_OK) {
        ESP_LOGE(TAG, "250-SPS data-rate configuration failed");
        return false;
    }

    if (ads1299_set_srb1(&g_device, 0) != ADS1299_OK) {
        ESP_LOGE(TAG, "EEG SRB1 disable failed");
        return false;
    }

    for (uint8_t channel = 1u; channel <= channel_count; ++channel) {
        if (ads1299_set_channel(&g_device,
                                channel,
                                ADS1299_GAIN_24,
                                ADS1299_MUX_NORMAL,
                                0,
                                0) != ADS1299_OK) {
            ESP_LOGE(TAG, "EEG CH%u configuration failed", (unsigned)channel);
            return false;
        }
    }

    ESP_LOGI(TAG,
             "250-SPS EEG profile ready: channels=%u gain=24 normal-input SRB1=off SRB2=off",
             (unsigned)channel_count);
    return true;
}

static void acquisition_task(void *arg)
{
    (void)arg;
    ads1299_frame_t frame;

    for (;;) {
        if (g_port.drdy_read(g_port.user) == 0) {
            if (ads1299_read_frame_continuous(&g_device, &frame) != ADS1299_OK) {
                ESP_LOGE(TAG, "frame read failed; acquisition task stopping");
                break;
            }

            const uint32_t timestamp_us = (uint32_t)esp_timer_get_time();
            const uint32_t sequence = g_sequence++;
            portENTER_CRITICAL(&g_queue_lock);
            (void)esp32c2_ads1299_frame_queue_push(
                &g_queue, &frame, timestamp_us, sequence);
            portEXIT_CRITICAL(&g_queue_lock);
        } else {
            vTaskDelay(pdMS_TO_TICKS(1));
        }
    }

    (void)ads1299_stop(&g_device);
    (void)ads1299_sdatac(&g_device);
    vTaskDelete(NULL);
}

static void transport_task(void *arg)
{
    (void)arg;
    esp32c2_ads1299_frame_record_t record;
    uint32_t last_reported_dropped = 0u;

    for (;;) {
        bool have_record;
        uint32_t dropped;
        size_t high_watermark;

        portENTER_CRITICAL(&g_queue_lock);
        have_record = esp32c2_ads1299_frame_queue_pop(&g_queue, &record);
        dropped = esp32c2_ads1299_frame_queue_dropped(&g_queue);
        high_watermark = esp32c2_ads1299_frame_queue_high_watermark(&g_queue);
        portEXIT_CRITICAL(&g_queue_lock);

        if (dropped != last_reported_dropped) {
            ESP_LOGW(TAG,
                     "acquisition queue overflow: dropped=%" PRIu32 " high_watermark=%u/%u",
                     dropped,
                     (unsigned)high_watermark,
                     (unsigned)ESP32C2_ADS1299_FRAME_QUEUE_CAPACITY);
            last_reported_dropped = dropped;
        }

        if (!have_record) {
            vTaskDelay(pdMS_TO_TICKS(1));
            continue;
        }

        /*
         * This is the deliberately lower-priority transport side. Replace the
         * ESP_LOGI call with UART/Wi-Fi/BLE/canonical-packet transmission as
         * needed; the DRDY/SPI acquisition task remains free of that latency.
         */
        if (record.sequence < 8u || (record.sequence % 250u) == 0u) {
            ESP_LOGI(TAG,
                     "EEG250 frame seq=%" PRIu32 " t=%" PRIu32
                     "us status=%02x%02x%02x ch1=%" PRId32,
                     record.sequence,
                     record.timestamp_us,
                     record.frame.status[0],
                     record.frame.status[1],
                     record.frame.status[2],
                     record.frame.channel[0]);
        }
    }
}

void app_main(void)
{
    ads1299_device_id_t identity;

    int rc = ads1299_espidf_hal_init(&g_idf_hal, &g_hal);
    if (rc != 0) {
        ESP_LOGE(TAG, "ESP-IDF HAL init failed: %d", rc);
        return;
    }
    if (ads1299_mcu_port_init(&g_mcu, &g_hal) != 0) {
        ESP_LOGE(TAG, "ADS1299 portable port init failed");
        return;
    }

    g_port = ads1299_mcu_make_port(&g_mcu);
    if (ads1299_init(&g_device, &g_port) != ADS1299_OK) {
        ESP_LOGE(TAG, "core init failed");
        return;
    }
    if (ads1299_hardware_reset(&g_device) != ADS1299_OK ||
        ads1299_sdatac(&g_device) != ADS1299_OK) {
        ESP_LOGE(TAG, "reset/SDATAC failed");
        return;
    }

    if (ads1299_read_device_id(&g_device, &identity) != ADS1299_OK) {
        ESP_LOGE(TAG, "ID read failed: check power, wiring and SPI mode 1");
        return;
    }
    if (!identity.is_ads1299_family) {
        ESP_LOGE(TAG, "unexpected ID 0x%02x: ADS1299 family not detected", identity.raw);
        return;
    }
    ESP_LOGI(TAG,
             "probe OK: ADS1299-family ID=0x%02x channels=%u",
             identity.raw,
             (unsigned)identity.channel_count);

    /* Progressive beginner diagnostics before electrode-input streaming. */
    if (!run_internal_test()) {
        return;
    }
    if (!run_input_short_test()) {
        return;
    }
    if (!configure_eeg_250(identity.channel_count)) {
        return;
    }
    if (!start_continuous("EEG250")) {
        return;
    }

    esp32c2_ads1299_frame_queue_init(&g_queue);
    g_sequence = 0u;

    ESP_LOGI(TAG,
             "250-SPS EEG streaming started: bounded queue=%u frames",
             (unsigned)ESP32C2_ADS1299_FRAME_QUEUE_CAPACITY);

    if (xTaskCreate(transport_task, "ads1299_tx", 3072, NULL, 3, NULL) != pdPASS) {
        ESP_LOGE(TAG, "transport task creation failed");
        (void)ads1299_stop(&g_device);
        (void)ads1299_sdatac(&g_device);
        return;
    }
    if (xTaskCreate(acquisition_task, "ads1299_acq", 3072, NULL, 5, NULL) != pdPASS) {
        ESP_LOGE(TAG, "acquisition task creation failed");
        (void)ads1299_stop(&g_device);
        (void)ads1299_sdatac(&g_device);
        return;
    }

    ESP_LOGI(TAG,
             "beginner flow complete: probe -> internal-test -> input-short -> EEG250 stream");
    ESP_LOGI(TAG,
             "acquisition runs above transport priority; slow UART/Wi-Fi/BLE work must stay in transport task");
}
