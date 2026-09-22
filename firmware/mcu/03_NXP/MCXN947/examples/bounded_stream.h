#ifndef MCXN947_ADS1299_BOUNDED_STREAM_H
#define MCXN947_ADS1299_BOUNDED_STREAM_H

#include <stdint.h>
#include "ads1299.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    ads1299_frame_t frame;
    uint32_t sequence;
} ads1299_stream_item_t;

/* Non-blocking foreground/transport consumer. Returns 1 when an item was
 * copied to out, 0 when the queue is empty or out is NULL. */
int ads1299_stream_try_pop(ads1299_stream_item_t *out);

/* Diagnostics for acquisition/transport sizing. The producer deliberately
 * drops the newest frame when its bounded queue is full. */
extern volatile uint32_t ads1299_stream_dropped;
extern volatile uint32_t ads1299_stream_produced;

#ifdef __cplusplus
}
#endif

#endif /* MCXN947_ADS1299_BOUNDED_STREAM_H */
