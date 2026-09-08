#ifndef ADS1299_FRAME_RING_H
#define ADS1299_FRAME_RING_H

#include <stddef.h>
#include <stdint.h>
#include "ads1299.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ADS1299_FRAME_RING_CAPACITY
#define ADS1299_FRAME_RING_CAPACITY 32u
#endif

typedef struct {
    uint32_t sequence;
    uint32_t timestamp_ticks;
    ads1299_frame_t frame;
} ads1299_buffered_frame_t;

typedef struct {
    ads1299_buffered_frame_t slots[ADS1299_FRAME_RING_CAPACITY];
    volatile size_t write_index;
    volatile size_t read_index;
    volatile size_t count;
    volatile uint32_t overflows;
    volatile uint32_t pushed;
    volatile uint32_t popped;
} ads1299_frame_ring_t;

void ads1299_frame_ring_init(ads1299_frame_ring_t *ring);
int ads1299_frame_ring_push(ads1299_frame_ring_t *ring,
                            const ads1299_frame_t *frame,
                            uint32_t sequence,
                            uint32_t timestamp_ticks);
int ads1299_frame_ring_pop(ads1299_frame_ring_t *ring,
                           ads1299_buffered_frame_t *out);
size_t ads1299_frame_ring_count(const ads1299_frame_ring_t *ring);
uint32_t ads1299_frame_ring_overflows(const ads1299_frame_ring_t *ring);

#ifdef __cplusplus
}
#endif

#endif
