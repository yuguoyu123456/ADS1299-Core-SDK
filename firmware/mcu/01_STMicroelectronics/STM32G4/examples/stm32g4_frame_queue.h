#ifndef STM32G4_ADS1299_FRAME_QUEUE_H
#define STM32G4_ADS1299_FRAME_QUEUE_H

#include <stddef.h>
#include <stdint.h>

#include "ads1299.h"

/*
 * Static bounded queue for sustained ADS1299 acquisition on STM32G4.
 *
 * Complete the full ADS1299 SPI transaction before enqueueing a decoded frame.
 * Packet encoding and slow UART/USB/network transport belong in a lower-priority
 * consumer so DRDY handling remains bounded. Overload is explicit: push() fails
 * and increments dropped instead of overwriting unread EEG data.
 *
 * Concurrency contract: one producer owns push(), one consumer owns pop(). If
 * producer and consumer can pre-empt one another, guard queue operations with
 * the application's critical-section primitive.
 */
#ifndef STM32G4_ADS1299_FRAME_QUEUE_CAPACITY
#define STM32G4_ADS1299_FRAME_QUEUE_CAPACITY 8u
#endif

#if STM32G4_ADS1299_FRAME_QUEUE_CAPACITY < 1
#error "STM32G4_ADS1299_FRAME_QUEUE_CAPACITY must be at least 1"
#endif

typedef struct {
    ads1299_frame_t frame;
    uint32_t timestamp_us;
    uint32_t sequence;
} stm32g4_ads1299_frame_record_t;

typedef struct {
    stm32g4_ads1299_frame_record_t slots[STM32G4_ADS1299_FRAME_QUEUE_CAPACITY];
    size_t head;
    size_t tail;
    size_t count;
    uint32_t dropped;
    uint32_t high_watermark;
} stm32g4_ads1299_frame_queue_t;

void stm32g4_ads1299_frame_queue_init(stm32g4_ads1299_frame_queue_t *queue);
int stm32g4_ads1299_frame_queue_push(stm32g4_ads1299_frame_queue_t *queue,
                                     const ads1299_frame_t *frame,
                                     uint32_t timestamp_us,
                                     uint32_t sequence);
int stm32g4_ads1299_frame_queue_pop(stm32g4_ads1299_frame_queue_t *queue,
                                    stm32g4_ads1299_frame_record_t *record);
size_t stm32g4_ads1299_frame_queue_count(const stm32g4_ads1299_frame_queue_t *queue);
uint32_t stm32g4_ads1299_frame_queue_dropped(const stm32g4_ads1299_frame_queue_t *queue);
uint32_t stm32g4_ads1299_frame_queue_high_watermark(const stm32g4_ads1299_frame_queue_t *queue);

#endif
