#ifndef STM32H563_ADS1299_FRAME_QUEUE_H
#define STM32H563_ADS1299_FRAME_QUEUE_H

#include <stddef.h>
#include <stdint.h>

#include "ads1299.h"

/*
 * Static bounded queue for sustained STM32H563 ADS1299 acquisition.
 * Stores decoded frames so acquisition timing is decoupled from UART/USB/
 * network packet transmission latency. No dynamic allocation is used.
 *
 * Concurrency contract: one producer owns push(), one consumer owns pop().
 * If they can pre-empt each other, protect push/pop with the application's
 * STM32 critical-section mechanism or call them from deferred contexts.
 */
#ifndef STM32H563_ADS1299_FRAME_QUEUE_CAPACITY
#define STM32H563_ADS1299_FRAME_QUEUE_CAPACITY 16u
#endif

typedef struct {
    ads1299_frame_t frame;
    uint32_t timestamp_us;
    uint32_t sequence;
} stm32h563_ads1299_frame_record_t;

typedef struct {
    stm32h563_ads1299_frame_record_t slots[STM32H563_ADS1299_FRAME_QUEUE_CAPACITY];
    size_t head;
    size_t tail;
    size_t count;
    uint32_t dropped;
    uint32_t high_watermark;
} stm32h563_ads1299_frame_queue_t;

void stm32h563_ads1299_frame_queue_init(stm32h563_ads1299_frame_queue_t *queue);
int stm32h563_ads1299_frame_queue_push(stm32h563_ads1299_frame_queue_t *queue,
                                       const ads1299_frame_t *frame,
                                       uint32_t timestamp_us,
                                       uint32_t sequence);
int stm32h563_ads1299_frame_queue_pop(stm32h563_ads1299_frame_queue_t *queue,
                                      stm32h563_ads1299_frame_record_t *record);
size_t stm32h563_ads1299_frame_queue_count(const stm32h563_ads1299_frame_queue_t *queue);
uint32_t stm32h563_ads1299_frame_queue_dropped(const stm32h563_ads1299_frame_queue_t *queue);
uint32_t stm32h563_ads1299_frame_queue_high_watermark(const stm32h563_ads1299_frame_queue_t *queue);

#endif
