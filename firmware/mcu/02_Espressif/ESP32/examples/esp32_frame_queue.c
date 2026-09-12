#include "esp32_frame_queue.h"

#include <string.h>

void esp32_ads1299_frame_queue_init(esp32_ads1299_frame_queue_t *q)
{
    if (q != NULL) {
        memset(q, 0, sizeof(*q));
    }
}

int esp32_ads1299_frame_queue_push(esp32_ads1299_frame_queue_t *q,
                                   const esp32_ads1299_frame_item_t *item)
{
    if (q == NULL || item == NULL) {
        return -1;
    }
    if (q->count >= ESP32_ADS1299_FRAME_QUEUE_CAPACITY) {
        ++q->dropped;
        return 0;
    }

    q->items[q->head] = *item;
    q->head = (q->head + 1u) % ESP32_ADS1299_FRAME_QUEUE_CAPACITY;
    ++q->count;
    if (q->count > q->high_watermark) {
        q->high_watermark = q->count;
    }
    return 1;
}

int esp32_ads1299_frame_queue_pop(esp32_ads1299_frame_queue_t *q,
                                  esp32_ads1299_frame_item_t *item)
{
    if (q == NULL || item == NULL) {
        return -1;
    }
    if (q->count == 0u) {
        return 0;
    }

    *item = q->items[q->tail];
    q->tail = (q->tail + 1u) % ESP32_ADS1299_FRAME_QUEUE_CAPACITY;
    --q->count;
    return 1;
}

size_t esp32_ads1299_frame_queue_count(const esp32_ads1299_frame_queue_t *q)
{
    return q == NULL ? 0u : q->count;
}

size_t esp32_ads1299_frame_queue_high_watermark(const esp32_ads1299_frame_queue_t *q)
{
    return q == NULL ? 0u : q->high_watermark;
}

uint32_t esp32_ads1299_frame_queue_dropped(const esp32_ads1299_frame_queue_t *q)
{
    return q == NULL ? 0u : q->dropped;
}
