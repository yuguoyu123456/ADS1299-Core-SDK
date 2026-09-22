#ifndef MCXA153_ADS1299_BOUNDED_STREAM_H
#define MCXA153_ADS1299_BOUNDED_STREAM_H

#include <stdint.h>
#include "ads1299.h"

/*
 * Transport-facing contract for the MCXA153 bounded acquisition example.
 * Keep UART/USB/network work outside the DRDY acquisition path. The producer
 * uses a fixed-depth queue and drops the newest frame on overflow.
 */
typedef struct {
    ads1299_frame_t frame;
    uint32_t sequence;
} ads1299_stream_item_t;

/* Non-blocking consumer: returns 1 when an item was copied, otherwise 0. */
int ads1299_stream_try_pop(ads1299_stream_item_t *out);

/* Diagnostics are monotonic counters for the lifetime of the example. */
extern volatile uint32_t ads1299_stream_dropped;
extern volatile uint32_t ads1299_stream_produced;

#endif /* MCXA153_ADS1299_BOUNDED_STREAM_H */
