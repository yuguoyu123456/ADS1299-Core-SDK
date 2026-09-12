#include "esp32c6_frame_queue.h"

#include <string.h>

void esp32c6_ads1299_frame_queue_init(esp32c6_ads1299_frame_queue_t *q)
{
    if (q != NULL) {
        memset(q, 0, sizeof(*q));
    }
}

bool esp32c6_ads1299_frame_queue_push(esp32c6_ads1299_frame_queue_t *q,
                                       const ads1299_frame_t *frame,
                                       uint32_t timestamp_us,
                                       uint32_t sequence)
{
    if (q == NULL || frame == NULL) return false;
    if (q->count >= ESP32C6_ADS1299_FRAME_QUEUE_CAPACITY) {
        ++q->dropped;
        return false;
    }
    esp32c6_ads1299_frame_record_t *slot = &q->items[q->head];
    slot->frame = *frame;
    slot->timestamp_us = timestamp_us;
    slot->sequence = sequence;
    q->head = (q->head + 1u) % ESP32C6_ADS1299_FRAME_QUEUE_CAPACITY;
    ++q->count;
    if (q->count > q->high_watermark) q->high_watermark = q->count;
    return true;
}

bool esp32c6_ads1299_frame_queue_pop(esp32c6_ads1299_frame_queue_t *q,
                                      esp32c6_ads1299_frame_record_t *out)
{
    if (q == NULL || out == NULL || q->count == 0u) return false;
    *out = q->items[q->tail];
    q->tail = (q->tail + 1u) % ESP32C6_ADS1299_FRAME_QUEUE_CAPACITY;
    --q->count;
    return true;
}

size_t esp32c6_ads1299_frame_queue_count(const esp32c6_ads1299_frame_queue_t *q)
{
    return q != NULL ? q->count : 0u;
}

size_t esp32c6_ads1299_frame_queue_high_watermark(const esp32c6_ads1299_frame_queue_t *q)
{
    return q != NULL ? q->high_watermark : 0u;
}

uint32_t esp32c6_ads1299_frame_queue_dropped(const esp32c6_ads1299_frame_queue_t *q)
{
    return q != NULL ? q->dropped : 0u;
}
