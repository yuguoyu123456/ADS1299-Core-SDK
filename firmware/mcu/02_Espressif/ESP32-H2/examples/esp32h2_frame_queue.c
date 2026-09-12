#include "esp32h2_frame_queue.h"
#include <string.h>

void esp32h2_ads1299_frame_queue_init(esp32h2_ads1299_frame_queue_t *q)
{
    if (q) memset(q, 0, sizeof(*q));
}

bool esp32h2_ads1299_frame_queue_push(esp32h2_ads1299_frame_queue_t *q,
                                       const ads1299_frame_t *frame,
                                       uint32_t timestamp_us,
                                       uint32_t sequence)
{
    if (!q || !frame) return false;
    if (q->count >= ESP32H2_ADS1299_FRAME_QUEUE_CAPACITY) {
        ++q->dropped;
        return false;
    }
    q->items[q->head].frame = *frame;
    q->items[q->head].timestamp_us = timestamp_us;
    q->items[q->head].sequence = sequence;
    q->head = (q->head + 1u) % ESP32H2_ADS1299_FRAME_QUEUE_CAPACITY;
    ++q->count;
    if (q->count > q->high_watermark) q->high_watermark = q->count;
    return true;
}

bool esp32h2_ads1299_frame_queue_pop(esp32h2_ads1299_frame_queue_t *q,
                                      esp32h2_ads1299_frame_record_t *out)
{
    if (!q || !out || q->count == 0u) return false;
    *out = q->items[q->tail];
    q->tail = (q->tail + 1u) % ESP32H2_ADS1299_FRAME_QUEUE_CAPACITY;
    --q->count;
    return true;
}

size_t esp32h2_ads1299_frame_queue_high_watermark(const esp32h2_ads1299_frame_queue_t *q)
{
    return q ? q->high_watermark : 0u;
}

uint32_t esp32h2_ads1299_frame_queue_dropped(const esp32h2_ads1299_frame_queue_t *q)
{
    return q ? q->dropped : 0u;
}
