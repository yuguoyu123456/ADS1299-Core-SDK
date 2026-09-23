#ifndef MCXW236_ADS1299_BOUNDED_STREAM_H
#define MCXW236_ADS1299_BOUNDED_STREAM_H

#include <stdint.h>
#include "ads1299.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Public consumer contract for the bounded acquisition example.
 * Transport code (UART/USB/radio/network) should drain this queue outside the
 * DRDY acquisition path so slow I/O cannot block ADS1299 frame capture. */
typedef struct {
    ads1299_frame_t frame;
    uint32_t sequence;
} ads1299_stream_item_t;

/* Non-blocking: returns 1 when an item was copied to out, otherwise 0. */
int ads1299_stream_try_pop(ads1299_stream_item_t *out);

/* Diagnostics. dropped increments when the bounded queue is full. */
extern volatile uint32_t ads1299_stream_dropped;
extern volatile uint32_t ads1299_stream_produced;

#ifdef __cplusplus
}
#endif

#endif /* MCXW236_ADS1299_BOUNDED_STREAM_H */
