#ifndef STM32F767_ADS1299_FRAME_QUEUE_H
#define STM32F767_ADS1299_FRAME_QUEUE_H

#include <stddef.h>
#include <stdint.h>

#include "ads1299.h"

/*
 * Static bounded queue for sustained STM32F767 ADS1299 acquisition.
 *
 * Complete and decode one ADS1299 frame before enqueueing it. Packet encoding
 * and UART/USB/Ethernet transport belong in a lower-priority consumer so that
 * transport latency does not block the DRDY acquisition path.
 *
 * No dynamic allocation is used. Overload is explicit: push() fails and the
 * dropped counter increments instead of overwriting unread EEG data.
 *
 * Concurrency contract: one producer owns push(), one consumer owns pop(). If
 * they can pre-empt one another, protect push/pop with the application's STM32
 * critical-section mechanism or defer both operations into compatible contexts.
 */
#ifndef STM32F767_ADS1299_FRAME_QUEUE_CAPACITY
#define STM32F767_ADS1299_FRAME_QUEUE_CAPACITY 16u
#endif

typedef struct {
    ads1299_frame_t frame;
    uint32_t timestamp_us;
    uint32_t sequence;
} stm32f767_ads1299_frame_record_t;

typedef struct {
    stm32f767_ads1299_frame_record_t slots[STM32F767_ADS1299_FRAME_QUEUE_CAPACITY];
    size_t head;
    size_t tail;
    size_t count;
    uint32_t dropped;
    uint32_t high_watermark;
} stm32f767_ads1299_frame_queue_t;

void stm32f767_ads1299_frame_queue_init(stm32f767_ads1299_frame_queue_t *queue);
int stm32f767_ads1299_frame_queue_push(stm32f767_ads1299_frame_queue_t *queue,
                                       const ads1299_frame_t *frame,
                                       uint32_t timestamp_us,
                                       uint32_t sequence);
int stm32f767_ads1299_frame_queue_pop(stm32f767_ads1299_frame_queue_t *queue,
                                      stm32f767_ads1299_frame_record_t *record);
size_t stm32f767_ads1299_frame_queue_count(const stm32f767_ads1299_frame_queue_t *queue);
uint32_t stm32f767_ads1299_frame_queue_dropped(const stm32f767_ads1299_frame_queue_t *queue);
uint32_t stm32f767_ads1299_frame_queue_high_watermark(const stm32f767_ads1299_frame_queue_t *queue);

#endif
