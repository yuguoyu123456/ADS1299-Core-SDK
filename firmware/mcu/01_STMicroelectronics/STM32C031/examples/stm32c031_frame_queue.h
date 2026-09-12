#ifndef STM32C031_ADS1299_FRAME_QUEUE_H
#define STM32C031_ADS1299_FRAME_QUEUE_H

#include <stddef.h>
#include <stdint.h>

#include "ads1299.h"

/*
 * Static bounded queue for STM32C031 sustained ADS1299 acquisition.
 *
 * Four decoded frames keep memory use deterministic while decoupling the
 * DRDY-critical SPI acquisition path from slower packet encoding and UART
 * transport. Applications may override the capacity at compile time after
 * checking their complete SRAM budget.
 *
 * Complete the entire ADS1299 SPI transaction before enqueueing a frame.
 * Packet encoding and transport belong in a lower-priority consumer. When
 * the queue is full, push() fails and increments dropped rather than silently
 * overwriting unread EEG data.
 *
 * Concurrency contract: one producer owns push(), one consumer owns pop(). If
 * they can pre-empt one another, protect queue operations with the application
 * critical-section mechanism.
 */
#ifndef STM32C031_ADS1299_FRAME_QUEUE_CAPACITY
#define STM32C031_ADS1299_FRAME_QUEUE_CAPACITY 4u
#endif

#if STM32C031_ADS1299_FRAME_QUEUE_CAPACITY < 1
#error "STM32C031_ADS1299_FRAME_QUEUE_CAPACITY must be at least 1"
#endif

typedef struct {
    ads1299_frame_t frame;
    uint32_t timestamp_us;
    uint32_t sequence;
} stm32c031_ads1299_frame_record_t;

typedef struct {
    stm32c031_ads1299_frame_record_t slots[STM32C031_ADS1299_FRAME_QUEUE_CAPACITY];
    size_t head;
    size_t tail;
    size_t count;
    uint32_t dropped;
    uint32_t high_watermark;
} stm32c031_ads1299_frame_queue_t;

void stm32c031_ads1299_frame_queue_init(stm32c031_ads1299_frame_queue_t *queue);
int stm32c031_ads1299_frame_queue_push(stm32c031_ads1299_frame_queue_t *queue,
                                       const ads1299_frame_t *frame,
                                       uint32_t timestamp_us,
                                       uint32_t sequence);
int stm32c031_ads1299_frame_queue_pop(stm32c031_ads1299_frame_queue_t *queue,
                                      stm32c031_ads1299_frame_record_t *record);
size_t stm32c031_ads1299_frame_queue_count(const stm32c031_ads1299_frame_queue_t *queue);
uint32_t stm32c031_ads1299_frame_queue_dropped(const stm32c031_ads1299_frame_queue_t *queue);
uint32_t stm32c031_ads1299_frame_queue_high_watermark(const stm32c031_ads1299_frame_queue_t *queue);

#endif
