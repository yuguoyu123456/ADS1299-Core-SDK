#ifndef MIMXRT1042_ADS1299_BOUNDED_STREAM_H
#define MIMXRT1042_ADS1299_BOUNDED_STREAM_H

#include <stdint.h>
#include "ads1299.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Foreground transport contract for the bounded acquisition example.
 * The DRDY acquisition path owns production; UART/USB/network code should
 * consume with ads1299_stream_try_pop() outside that timing-critical path. */
typedef struct {
    ads1299_frame_t frame;
    uint32_t sequence;
} ads1299_stream_item_t;

/* Non-blocking consumer API: returns 1 when an item was copied to out,
 * otherwise 0. Passing NULL also returns 0. */
int ads1299_stream_try_pop(ads1299_stream_item_t *out);

/* Diagnostics. produced counts accepted frames; dropped counts frames rejected
 * because the fixed queue was full. These counters are intentionally simple
 * example diagnostics, not synchronization primitives. */
extern volatile uint32_t ads1299_stream_produced;
extern volatile uint32_t ads1299_stream_dropped;

#ifdef __cplusplus
}
#endif

#endif /* MIMXRT1042_ADS1299_BOUNDED_STREAM_H */
