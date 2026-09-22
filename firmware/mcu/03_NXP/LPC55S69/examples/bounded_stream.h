#ifndef LPC55S69_ADS1299_BOUNDED_STREAM_H
#define LPC55S69_ADS1299_BOUNDED_STREAM_H

#include <stdint.h>
#include "ads1299.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Public consumer contract for the bounded acquisition example.
 * Transport code (UART/USB/network) should drain this queue outside the
 * ADS1299 acquisition/DRDY timing path. */
typedef struct {
    ads1299_frame_t frame;
    uint32_t sequence;
} ads1299_stream_item_t;

/* Non-blocking consumer API: returns 1 when an item was copied, otherwise 0. */
int ads1299_stream_try_pop(ads1299_stream_item_t *out);

/* Diagnostics. dropped increments when the fixed queue is full and the newest
 * frame is discarded; produced is the sequence source for accepted frames. */
extern volatile uint32_t ads1299_stream_dropped;
extern volatile uint32_t ads1299_stream_produced;

#ifdef __cplusplus
}
#endif

#endif /* LPC55S69_ADS1299_BOUNDED_STREAM_H */
