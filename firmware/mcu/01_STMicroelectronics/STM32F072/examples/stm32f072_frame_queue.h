#ifndef STM32F072_ADS1299_FRAME_QUEUE_H
#define STM32F072_ADS1299_FRAME_QUEUE_H

#include <stddef.h>
#include <stdint.h>

#include "ads1299.h"

/*
 * Small, static bounded queue for STM32F072 sustained ADS1299 acquisition.
 *
 * STM32F0-class parts are RAM-constrained, so the default is intentionally
 * only four complete decoded frames. Applications may override the capacity
 * at compile time, but should account for the full queue object in SRAM.
 *
 * Complete one ADS1299 frame before enqueueing it. Packet encoding and slow
 * UART/USB transport belong in a consumer outside the DRDY-critical path.
 * Overload is explicit: push() fails and increments dropped rather than
 * overwriting unread EEG data.
 *
 * Concurrency contract: one producer owns push(), one consumer owns pop(). If
 * producer and consumer can pre-empt one another, protect queue operations with
 * the application's critical-section mechanism.
 */
#ifndef STM32F072_ADS1299_FRAME_QUEUE_CAPACITY
#define STM32F072_ADS1299_FRAME_QUEUE_CAPACITY 4u
#endif

#if STM32F072_ADS1299_FRAME_QUEUE_CAPACITY < 1
#error "STM32F072_ADS1299_FRAME_QUEUE_CAPACITY must be at least 1"
#endif

typedef struct {
    ads1299_frame_t frame;
    uint32_t timestamp_us;
    uint32_t sequence;
} stm32f072_ads1299_frame_record_t;

typedef struct {
    stm32f072_ads1299_frame_record_t slots[STM32F072_ADS1299_FRAME_QUEUE_CAPACITY];
    size_t head;
    size_t tail;
    size_t count;
    uint32_t dropped;
    uint32_t high_watermark;
} stm32f072_ads1299_frame_queue_t;

void stm32f072_ads1299_frame_queue_init(stm32f072_ads1299_frame_queue_t *queue);
int stm32f072_ads1299_frame_queue_push(stm32f072_ads1299_frame_queue_t *queue,
                                       const ads1299_frame_t *frame,
                                       uint32_t timestamp_us,
                                       uint32_t sequence);
int stm32f072_ads1299_frame_queue_pop(stm32f072_ads1299_frame_queue_t *queue,
                                      stm32f072_ads1299_frame_record_t *record);
size_t stm32f072_ads1299_frame_queue_count(const stm32f072_ads1299_frame_queue_t *queue);
uint32_t stm32f072_ads1299_frame_queue_dropped(const stm32f072_ads1299_frame_queue_t *queue);
uint32_t stm32f072_ads1299_frame_queue_high_watermark(const stm32f072_ads1299_frame_queue_t *queue);

#endif
