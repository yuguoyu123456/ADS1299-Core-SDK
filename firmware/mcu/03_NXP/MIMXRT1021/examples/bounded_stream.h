#ifndef MIMXRT1021_ADS1299_BOUNDED_STREAM_H
#define MIMXRT1021_ADS1299_BOUNDED_STREAM_H

#include <stdint.h>
#include "ads1299.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Fixed-size handoff item between the ADS1299 acquisition path and a
 * foreground UART/USB/network transport.  The acquisition side never waits
 * for the consumer; overflow is counted by ads1299_stream_dropped. */
typedef struct {
    ads1299_frame_t frame;
    uint32_t sequence;
} ads1299_stream_item_t;

/* Non-blocking foreground consumer API. Returns 1 when an item was copied,
 * 0 when the queue is empty (or out is NULL). */
int ads1299_stream_try_pop(ads1299_stream_item_t *out);

/* Diagnostics. produced is the sequence source for accepted frames; dropped
 * counts frames rejected because the bounded queue was full. */
extern volatile uint32_t ads1299_stream_dropped;
extern volatile uint32_t ads1299_stream_produced;

#ifdef __cplusplus
}
#endif

#endif /* MIMXRT1021_ADS1299_BOUNDED_STREAM_H */
