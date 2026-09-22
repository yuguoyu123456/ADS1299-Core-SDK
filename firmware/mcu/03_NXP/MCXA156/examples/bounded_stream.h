#ifndef MCXA156_ADS1299_BOUNDED_STREAM_H
#define MCXA156_ADS1299_BOUNDED_STREAM_H

#include <stdint.h>
#include "ads1299.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Transport-facing item produced by the bounded ADS1299 acquisition example.
 * Keep transport work outside the DRDY/frame-read path. */
typedef struct {
    ads1299_frame_t frame;
    uint32_t sequence;
} ads1299_stream_item_t;

/* Non-blocking consumer API: returns 1 when an item was copied, 0 otherwise. */
int ads1299_stream_try_pop(ads1299_stream_item_t *out);

/* Diagnostics. dropped counts frames rejected when the fixed queue is full;
 * produced is the sequence source for frames accepted into the queue. */
extern volatile uint32_t ads1299_stream_dropped;
extern volatile uint32_t ads1299_stream_produced;

#ifdef __cplusplus
}
#endif

#endif /* MCXA156_ADS1299_BOUNDED_STREAM_H */
