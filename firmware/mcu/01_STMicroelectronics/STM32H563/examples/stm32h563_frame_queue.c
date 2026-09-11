#include "stm32h563_frame_queue.h"

#include <string.h>

void stm32h563_ads1299_frame_queue_init(stm32h563_ads1299_frame_queue_t *queue)
{
    if (queue != NULL) {
        memset(queue, 0, sizeof(*queue));
    }
}

int stm32h563_ads1299_frame_queue_push(stm32h563_ads1299_frame_queue_t *queue,
                                       const ads1299_frame_t *frame,
                                       uint32_t timestamp_us,
                                       uint32_t sequence)
{
    stm32h563_ads1299_frame_record_t *slot;

    if (queue == NULL || frame == NULL || STM32H563_ADS1299_FRAME_QUEUE_CAPACITY == 0u) {
        return -1;
    }
    if (queue->count >= STM32H563_ADS1299_FRAME_QUEUE_CAPACITY) {
        ++queue->dropped;
        return -1;
    }

    slot = &queue->slots[queue->head];
    slot->frame = *frame;
    slot->timestamp_us = timestamp_us;
    slot->sequence = sequence;
    queue->head = (queue->head + 1u) % STM32H563_ADS1299_FRAME_QUEUE_CAPACITY;
    ++queue->count;
    if (queue->count > queue->high_watermark) {
        queue->high_watermark = (uint32_t)queue->count;
    }
    return 0;
}

int stm32h563_ads1299_frame_queue_pop(stm32h563_ads1299_frame_queue_t *queue,
                                      stm32h563_ads1299_frame_record_t *record)
{
    if (queue == NULL || record == NULL || queue->count == 0u ||
        STM32H563_ADS1299_FRAME_QUEUE_CAPACITY == 0u) {
        return -1;
    }

    *record = queue->slots[queue->tail];
    queue->tail = (queue->tail + 1u) % STM32H563_ADS1299_FRAME_QUEUE_CAPACITY;
    --queue->count;
    return 0;
}

size_t stm32h563_ads1299_frame_queue_count(const stm32h563_ads1299_frame_queue_t *queue)
{
    return queue == NULL ? 0u : queue->count;
}

uint32_t stm32h563_ads1299_frame_queue_dropped(const stm32h563_ads1299_frame_queue_t *queue)
{
    return queue == NULL ? 0u : queue->dropped;
}

uint32_t stm32h563_ads1299_frame_queue_high_watermark(const stm32h563_ads1299_frame_queue_t *queue)
{
    return queue == NULL ? 0u : queue->high_watermark;
}
