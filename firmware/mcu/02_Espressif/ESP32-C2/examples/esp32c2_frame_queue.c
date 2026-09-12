#include "esp32c2_frame_queue.h"

#include <string.h>

void esp32c2_ads1299_frame_queue_init(esp32c2_ads1299_frame_queue_t *queue)
{
    if (queue == NULL) {
        return;
    }
    memset(queue, 0, sizeof(*queue));
}

bool esp32c2_ads1299_frame_queue_push(
    esp32c2_ads1299_frame_queue_t *queue,
    const ads1299_frame_t *frame,
    uint32_t timestamp_us,
    uint32_t sequence)
{
    if (queue == NULL || frame == NULL) {
        return false;
    }
    if (queue->count >= ESP32C2_ADS1299_FRAME_QUEUE_CAPACITY) {
        ++queue->dropped;
        return false;
    }

    esp32c2_ads1299_frame_record_t *slot = &queue->items[queue->head];
    slot->frame = *frame;
    slot->timestamp_us = timestamp_us;
    slot->sequence = sequence;

    queue->head = (queue->head + 1u) % ESP32C2_ADS1299_FRAME_QUEUE_CAPACITY;
    ++queue->count;
    if (queue->count > queue->high_watermark) {
        queue->high_watermark = queue->count;
    }
    return true;
}

bool esp32c2_ads1299_frame_queue_pop(
    esp32c2_ads1299_frame_queue_t *queue,
    esp32c2_ads1299_frame_record_t *out)
{
    if (queue == NULL || out == NULL || queue->count == 0u) {
        return false;
    }

    *out = queue->items[queue->tail];
    queue->tail = (queue->tail + 1u) % ESP32C2_ADS1299_FRAME_QUEUE_CAPACITY;
    --queue->count;
    return true;
}

size_t esp32c2_ads1299_frame_queue_count(const esp32c2_ads1299_frame_queue_t *queue)
{
    return queue == NULL ? 0u : queue->count;
}

size_t esp32c2_ads1299_frame_queue_high_watermark(const esp32c2_ads1299_frame_queue_t *queue)
{
    return queue == NULL ? 0u : queue->high_watermark;
}

uint32_t esp32c2_ads1299_frame_queue_dropped(const esp32c2_ads1299_frame_queue_t *queue)
{
    return queue == NULL ? 0u : queue->dropped;
}
