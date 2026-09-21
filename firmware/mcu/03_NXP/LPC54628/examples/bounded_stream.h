#ifndef ADS1299_LPC54628_BOUNDED_STREAM_H
#define ADS1299_LPC54628_BOUNDED_STREAM_H

#include <stdint.h>
#include "ads1299.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Public consumer contract for the bounded acquisition example.
 * Transport code (UART/USB/network) should drain this queue outside the
 * DRDY acquisition path. The producer uses a fixed-size queue and drops the
 * newest frame when full rather than blocking acquisition. */
typedef struct {
    ads1299_frame_t frame;
    uint32_t sequence;
} ads1299_stream_item_t;

/* Returns 1 and copies one queued frame to *out when data is available.
 * Returns 0 for an empty queue or a NULL output pointer. */
int ads1299_stream_try_pop(ads1299_stream_item_t *out);

/* Diagnostics exported by bounded_stream.c.
 * produced counts frames accepted into the queue; dropped counts frames
 * rejected because the fixed queue was full. */
extern volatile uint32_t ads1299_stream_dropped;
extern volatile uint32_t ads1299_stream_produced;

#ifdef __cplusplus
}
#endif

#endif /* ADS1299_LPC54628_BOUNDED_STREAM_H */
