#ifndef STM32H533_ADS1299_FRAME_QUEUE_H
#define STM32H533_ADS1299_FRAME_QUEUE_H

#include <stddef.h>
#include <stdint.h>

#include "ads1299.h"

/*
 * Small, static, bounded queue for sustained STM32H533 acquisition.
 *
 * The queue deliberately stores decoded ADS1299 frames rather than transport
 * packets.  Acquisition can therefore remain independent from USB/UART/etc.
 * packet transmission latency.  No dynamic allocation is used.
 *
 * Concurrency contract:
 *   - one producer context owns push();
 *   - one consumer context owns pop();
 *   - if producer and consumer can pre-empt one another, protect push/pop with
 *     the application's normal STM32 critical-section mechanism or call them
 *     from deferred/non-ISR contexts.
 *
 * The queue does not pretend to be a universal lock-free ISR primitive.
 */

#ifndef STM32H533_ADS1299_FRAME_QUEUE_CAPACITY
#define STM32H533_ADS1299_FRAME_QUEUE_CAPACITY 16u
#endif

typedef struct {
    ads1299_frame_t frame;
    uint32_t timestamp_us;
    uint32_t sequence;
} stm32h533_ads1299_frame_record_t;

typedef struct {
    stm32h533_ads1299_frame_record_t slots[STM32H533_ADS1299_FRAME_QUEUE_CAPACITY];
    size_t head;
    size_t tail;
    size_t count;
    uint32_t dropped;
    uint32_t high_watermark;
} stm32h533_ads1299_frame_queue_t;

void stm32h533_ads1299_frame_queue_init(stm32h533_ads1299_frame_queue_t *queue);

/* Returns 0 on success and -1 when the bounded queue is full/invalid. */
int stm32h533_ads1299_frame_queue_push(
    stm32h533_ads1299_frame_queue_t *queue,
    const ads1299_frame_t *frame,
    uint32_t timestamp_us,
    uint32_t sequence);

/* Returns 0 on success and -1 when the queue is empty/invalid. */
int stm32h533_ads1299_frame_queue_pop(
    stm32h533_ads1299_frame_queue_t *queue,
    stm32h533_ads1299_frame_record_t *record);

size_t stm32h533_ads1299_frame_queue_count(
    const stm32h533_ads1299_frame_queue_t *queue);

uint32_t stm32h533_ads1299_frame_queue_dropped(
    const stm32h533_ads1299_frame_queue_t *queue);

uint32_t stm32h533_ads1299_frame_queue_high_watermark(
    const stm32h533_ads1299_frame_queue_t *queue);

#endif /* STM32H533_ADS1299_FRAME_QUEUE_H */
