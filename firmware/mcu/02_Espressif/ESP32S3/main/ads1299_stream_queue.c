#include "ads1299_stream_queue.h"

#include <string.h>

void esp32s3_ads1299_stream_queue_init(esp32s3_ads1299_stream_queue_t *queue) {
    if (queue == NULL) {
        return;
    }
    memset(queue, 0, sizeof(*queue));
}

int esp32s3_ads1299_stream_queue_push(
    esp32s3_ads1299_stream_queue_t *queue,
    const esp32s3_ads1299_stream_item_t *item) {
    if (queue == NULL || item == NULL) {
        return -1;
    }
    if (queue->count >= ESP32S3_ADS1299_STREAM_QUEUE_CAPACITY) {
        queue->dropped++;
        return 0;
    }

    queue->items[queue->head] = *item;
    queue->head = (queue->head + 1u) % ESP32S3_ADS1299_STREAM_QUEUE_CAPACITY;
    queue->count++;
    if (queue->count > queue->high_watermark) {
        queue->high_watermark = queue->count;
    }
    return 1;
}

int esp32s3_ads1299_stream_queue_pop(
    esp32s3_ads1299_stream_queue_t *queue,
    esp32s3_ads1299_stream_item_t *item) {
    if (queue == NULL || item == NULL) {
        return -1;
    }
    if (queue->count == 0u) {
        return 0;
    }

    *item = queue->items[queue->tail];
    queue->tail = (queue->tail + 1u) % ESP32S3_ADS1299_STREAM_QUEUE_CAPACITY;
    queue->count--;
    return 1;
}

size_t esp32s3_ads1299_stream_queue_count(
    const esp32s3_ads1299_stream_queue_t *queue) {
    return queue == NULL ? 0u : queue->count;
}

size_t esp32s3_ads1299_stream_queue_high_watermark(
    const esp32s3_ads1299_stream_queue_t *queue) {
    return queue == NULL ? 0u : queue->high_watermark;
}

uint32_t esp32s3_ads1299_stream_queue_dropped(
    const esp32s3_ads1299_stream_queue_t *queue) {
    return queue == NULL ? 0u : queue->dropped;
}
