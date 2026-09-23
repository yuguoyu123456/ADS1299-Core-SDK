#ifndef ADS1299_MIMXRT1011_BOUNDED_STREAM_H
#define ADS1299_MIMXRT1011_BOUNDED_STREAM_H

#include <stdint.h>
#include "ads1299.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Public foreground-consumer contract for bounded_stream.c.
 * Acquisition owns the producer side; transports should only consume here.
 * Keep UART/USB/network blocking work outside the DRDY acquisition path. */
typedef struct {
    ads1299_frame_t frame;
    uint32_t sequence;
} ads1299_stream_item_t;

/* Non-blocking dequeue: 1 = item returned, 0 = queue empty/invalid output. */
int ads1299_stream_try_pop(ads1299_stream_item_t *out);

/* Diagnostics. dropped increments when the fixed queue is full; the producer
 * intentionally drops the newest frame rather than blocking acquisition. */
extern volatile uint32_t ads1299_stream_dropped;
extern volatile uint32_t ads1299_stream_produced;

#ifdef __cplusplus
}
#endif

#endif /* ADS1299_MIMXRT1011_BOUNDED_STREAM_H */
