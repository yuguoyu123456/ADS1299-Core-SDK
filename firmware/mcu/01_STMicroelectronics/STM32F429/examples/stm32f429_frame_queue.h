#ifndef STM32F429_ADS1299_FRAME_QUEUE_H
#define STM32F429_ADS1299_FRAME_QUEUE_H

#include <stddef.h>
#include <stdint.h>

#include "ads1299.h"

/*
 * Static bounded queue for STM32F429 sustained ADS1299 acquisition.
 *
 * Eight complete decoded frames provide bounded transport headroom while
 * keeping memory use deterministic. Applications may override the capacity at
 * compile time after checking the full SRAM budget for their transport stack.
 *
 * Complete the entire ADS1299 SPI transaction before enqueueing a frame.
 * Packet encoding plus UART/USB/Ethernet transport belongs in a lower-priority
 * consumer rather than the DRDY-critical path. Overload is explicit: push()
 * fails and increments dropped instead of overwriting unread EEG data.
 *
 * Concurrency contract: one producer owns push(), one consumer owns pop(). If
 * they can pre-empt one another, protect queue operations with the application
 * critical-section mechanism.
 */
#ifndef STM32F429_ADS1299_FRAME_QUEUE_CAPACITY
#define STM32F429_ADS1299_FRAME_QUEUE_CAPACITY 8u
#endif

#if STM32F429_ADS1299_FRAME_QUEUE_CAPACITY < 1
#error "STM32F429_ADS1299_FRAME_QUEUE_CAPACITY must be at least 1"
#endif

typedef struct {
    ads1299_frame_t frame;
    uint32_t timestamp_us;
    uint32_t sequence;
} stm32f429_ads1299_frame_record_t;

typedef struct {
    stm32f429_ads1299_frame_record_t slots[STM32F429_ADS1299_FRAME_QUEUE_CAPACITY];
    size_t head;
    size_t tail;
    size_t count;
    uint32_t dropped;
    uint32_t high_watermark;
} stm32f429_ads1299_frame_queue_t;

void stm32f429_ads1299_frame_queue_init(stm32f429_ads1299_frame_queue_t *queue);
int stm32f429_ads1299_frame_queue_push(stm32f429_ads1299_frame_queue_t *queue,
                                       const ads1299_frame_t *frame,
                                       uint32_t timestamp_us,
                                       uint32_t sequence);
int stm32f429_ads1299_frame_queue_pop(stm32f429_ads1299_frame_queue_t *queue,
                                      stm32f429_ads1299_frame_record_t *record);
size_t stm32f429_ads1299_frame_queue_count(const stm32f429_ads1299_frame_queue_t *queue);
uint32_t stm32f429_ads1299_frame_queue_dropped(const stm32f429_ads1299_frame_queue_t *queue);
uint32_t stm32f429_ads1299_frame_queue_high_watermark(const stm32f429_ads1299_frame_queue_t *queue);

#endif
