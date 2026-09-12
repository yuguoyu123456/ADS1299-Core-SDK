/* ESP32-C6 progressive ADS1299 reference flow.
 *
 * Hardware-dependent SPI/GPIO ownership stays in board/ and esp_idf_adapter/.
 * ADS1299 register semantics stay in the shared core.  This example keeps the
 * historical latest-frame debugger snapshot while adding a bounded producer /
 * consumer path so slower transport work never runs in the acquisition loop.
 */
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>

#include "ads1299.h"
#include "ads1299_spi.h"
#include "esp32c6_frame_queue.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern int board_ads1299_hal(ads1299_platform_hal_t *hal);

/* Backward-compatible debugger snapshot. Odd sequence means writing. */
volatile ads1299_frame_t ads1299_latest_frame;
volatile uint32_t ads1299_frame_sequence;

static const char *TAG = "ads1299-c6";
static ads1299_mcu_port_t g_mcu;
static ads1299_port_t g_port;
static ads1299_t g_device;
static esp32c6_ads1299_frame_queue_t g_queue;
static portMUX_TYPE g_queue_lock = portMUX_INITIALIZER_UNLOCKED;
static uint32_t g_stream_sequence;

#define DIAG_FRAMES 8u
#define DRDY_TIMEOUT_US 500000u
#define DRDY_POLL_US 100u

static bool start_continuous(const char *phase)
{
    ads1299_status_t rdatac = ads1299_rdatac(&g_device);
    ads1299_status_t start = ads1299_start(&g_device);
    if (rdatac != ADS1299_OK || start != ADS1299_OK) {
        ESP_LOGE(TAG, "%s start failed: rdatac=%d start=%d",
                 phase, (int)rdatac, (int)start);
        return false;
    }
    return true;
}

static bool stop_continuous(const char *phase)
{
    ads1299_status_t stop = ads1299_stop(&g_device);
    ads1299_status_t sdatac = ads1299_sdatac(&g_device);
    if (stop != ADS1299_OK || sdatac != ADS1299_OK) {
        ESP_LOGE(TAG, "%s stop failed: stop=%d sdatac=%d",
                 phase, (int)stop, (int)sdatac);
        return false;
    }
    return true;
}

static bool capture_frames(const char *phase)
{
    ads1299_frame_t frame;

    for (uint32_t i = 0; i < DIAG_FRAMES; ++i) {
        ads1299_status_t rc = ads1299_wait_drdy(&g_device,
                                                DRDY_TIMEOUT_US,
                                                DRDY_POLL_US);
        if (rc != ADS1299_OK) {
            ESP_LOGE(TAG, "%s DRDY timeout/error at frame %" PRIu32
                          ": rc=%d",
                     phase, i, (int)rc);
            return false;
        }

        rc = ads1299_read_frame_continuous(&g_device, &frame);
        if (rc != ADS1299_OK) {
            ESP_LOGE(TAG, "%s frame read failed at frame %" PRIu32
                          ": rc=%d",
                     phase, i, (int)rc);
            return false;
        }

        ESP_LOGI(TAG,
                 "%s frame=%" PRIu32 " status=%02x%02x%02x ch1=%" PRId32,
                 phase, i, frame.status[0], frame.status[1], frame.status[2],
                 frame.channel[0]);
    }

    return true;
}

static bool run_internal_test(void)
{
    ads1299_status_t rc = ads1299_configure_internal_test(
        &g_device, ADS1299_GAIN_24, 0, ADS1299_TEST_FREQ_FCLK_DIV_2_21);
    if (rc != ADS1299_OK) {
        ESP_LOGE(TAG, "internal-test configuration failed: rc=%d", (int)rc);
        return false;
    }
    if (!start_continuous("internal-test")) return false;

    bool ok = capture_frames("internal-test");
    return stop_continuous("internal-test") && ok;
}

static bool run_input_short(void)
{
    ads1299_status_t rc = ads1299_configure_input_short_test(
        &g_device, ADS1299_GAIN_24);
    if (rc != ADS1299_OK) {
        ESP_LOGE(TAG, "input-short configuration failed: rc=%d", (int)rc);
        return false;
    }
    if (!start_continuous("input-short")) return false;

    bool ok = capture_frames("input-short");
    return stop_continuous("input-short") && ok;
}

static bool configure_eeg250(uint8_t channels)
{
    ads1299_status_t rc = ads1299_set_data_rate(&g_device, ADS1299_DR_250SPS);
    if (rc != ADS1299_OK) {
        ESP_LOGE(TAG, "250-SPS data-rate configuration failed: rc=%d", (int)rc);
        return false;
    }

    rc = ads1299_set_srb1(&g_device, 0);
    if (rc != ADS1299_OK) {
        ESP_LOGE(TAG, "SRB1 configuration failed: rc=%d", (int)rc);
        return false;
    }

    for (uint8_t ch = 1u; ch <= channels; ++ch) {
        rc = ads1299_set_channel(&g_device, ch, ADS1299_GAIN_24,
                                 ADS1299_MUX_NORMAL, 0, 0);
        if (rc != ADS1299_OK) {
            ESP_LOGE(TAG, "EEG250 channel %u configuration failed: rc=%d",
                     (unsigned)ch, (int)rc);
            return false;
        }
    }

    return true;
}

static void acquisition_task(void *arg)
{
    (void)arg;
    ads1299_frame_t frame;

    for (;;) {
        int ready = g_port.drdy_read(g_port.user);
        if (ready < 0) {
            ESP_LOGE(TAG, "DRDY read failed in acquisition task");
            break;
        }
        if (ready != 0) {
            taskYIELD();
            continue;
        }

        ads1299_status_t rc = ads1299_read_frame_continuous(&g_device, &frame);
        if (rc != ADS1299_OK) {
            ESP_LOGE(TAG, "continuous frame read failed: rc=%d", (int)rc);
            break;
        }

        uint32_t timestamp_us = (uint32_t)esp_timer_get_time();
        uint32_t sequence = g_stream_sequence++;

        /* Preserve the historical debugger snapshot without making transport
         * consume it.  Odd ads1299_frame_sequence means an update is active. */
        ++ads1299_frame_sequence;
        ads1299_latest_frame = frame;
        ++ads1299_frame_sequence;

        portENTER_CRITICAL(&g_queue_lock);
        (void)esp32c6_ads1299_frame_queue_push(&g_queue, &frame,
                                                timestamp_us, sequence);
        portEXIT_CRITICAL(&g_queue_lock);
    }

    (void)ads1299_stop(&g_device);
    (void)ads1299_sdatac(&g_device);
    vTaskDelete(NULL);
}

static void transport_task(void *arg)
{
    (void)arg;
    esp32c6_ads1299_frame_record_t rec;
    uint32_t last_dropped = 0u;

    for (;;) {
        bool have;
        uint32_t dropped;
        size_t high_watermark;
        size_t depth;

        portENTER_CRITICAL(&g_queue_lock);
        have = esp32c6_ads1299_frame_queue_pop(&g_queue, &rec);
        dropped = esp32c6_ads1299_frame_queue_dropped(&g_queue);
        high_watermark = esp32c6_ads1299_frame_queue_high_watermark(&g_queue);
        depth = esp32c6_ads1299_frame_queue_count(&g_queue);
        portEXIT_CRITICAL(&g_queue_lock);

        if (dropped != last_dropped) {
            ESP_LOGW(TAG,
                     "queue overflow dropped=%" PRIu32 " depth=%u high=%u/%u",
                     dropped, (unsigned)depth, (unsigned)high_watermark,
                     (unsigned)ESP32C6_ADS1299_FRAME_QUEUE_CAPACITY);
            last_dropped = dropped;
        }

        if (!have) {
            vTaskDelay(pdMS_TO_TICKS(1));
            continue;
        }

        /* This log is intentionally sparse. Replace this block with UART,
         * Wi-Fi, BLE or Thread packet transport; never move it into the DRDY
         * acquisition path. */
        if (rec.sequence < 8u || (rec.sequence % 250u) == 0u) {
            ESP_LOGI(TAG,
                     "EEG250 seq=%" PRIu32 " t=%" PRIu32
                     " status=%02x%02x%02x ch1=%" PRId32,
                     rec.sequence, rec.timestamp_us,
                     rec.frame.status[0], rec.frame.status[1],
                     rec.frame.status[2], rec.frame.channel[0]);
        }
    }
}

int main(void)
{
    ads1299_platform_hal_t hal;
    ads1299_device_id_t identity;
    TaskHandle_t transport_handle = NULL;

    if (board_ads1299_hal(&hal) != 0) {
        ESP_LOGE(TAG, "board/HAL initialization failed");
        return 1;
    }
    if (ads1299_mcu_port_init(&g_mcu, &hal) != 0) {
        ESP_LOGE(TAG, "portable MCU port initialization failed");
        return 2;
    }

    g_port = ads1299_mcu_make_port(&g_mcu);
    if (ads1299_init(&g_device, &g_port) != ADS1299_OK) {
        ESP_LOGE(TAG, "shared ADS1299 core initialization failed");
        return 3;
    }
    if (ads1299_hardware_reset(&g_device) != ADS1299_OK) {
        ESP_LOGE(TAG, "ADS1299 hardware reset failed");
        return 4;
    }
    if (ads1299_sdatac(&g_device) != ADS1299_OK) {
        ESP_LOGE(TAG, "initial SDATAC failed");
        return 5;
    }

    if (ads1299_read_device_id(&g_device, &identity) != ADS1299_OK ||
        !identity.is_ads1299_family) {
        ESP_LOGE(TAG, "probe failed: check power/wiring/SPI mode 1");
        return 6;
    }
    ESP_LOGI(TAG, "probe OK: ADS1299-family ID=0x%02x channels=%u",
             identity.raw, (unsigned)identity.channel_count);

    if (!run_internal_test()) return 7;
    if (!run_input_short()) return 8;
    if (!configure_eeg250(identity.channel_count)) return 9;
    if (!start_continuous("EEG250")) return 10;

    esp32c6_ads1299_frame_queue_init(&g_queue);
    g_stream_sequence = 0u;
    ads1299_frame_sequence = 0u;

    if (xTaskCreate(transport_task, "ads1299_tx", 3072, NULL, 3,
                    &transport_handle) != pdPASS) {
        ESP_LOGE(TAG, "transport task creation failed");
        (void)stop_continuous("EEG250");
        return 11;
    }

    if (xTaskCreate(acquisition_task, "ads1299_acq", 3072, NULL, 5,
                    NULL) != pdPASS) {
        ESP_LOGE(TAG, "acquisition task creation failed");
        vTaskDelete(transport_handle);
        (void)stop_continuous("EEG250");
        return 12;
    }

    ESP_LOGI(TAG,
             "beginner flow complete: probe -> internal-test -> input-short -> EEG250 stream");
    return 0;
}
