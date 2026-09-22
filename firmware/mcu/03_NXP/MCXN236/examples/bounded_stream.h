#ifndef MCXN236_ADS1299_BOUNDED_STREAM_H
#define MCXN236_ADS1299_BOUNDED_STREAM_H

#include <stdint.h>
#include "ads1299.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Public consumer contract for the MCXN236 bounded acquisition example.
 * Acquisition remains independent of UART/USB/network transport. */
typedef struct {
    ads1299_frame_t frame;
    uint32_t sequence;
} ads1299_stream_item_t;

/* Non-blocking foreground/transport consumer. Returns 1 when an item was
 * copied to out, otherwise 0. */
int ads1299_stream_try_pop(ads1299_stream_item_t *out);

/* Diagnostics: produced counts accepted frames; dropped counts frames rejected
 * because the fixed queue was full. */
extern volatile uint32_t ads1299_stream_dropped;
extern volatile uint32_t ads1299_stream_produced;

#ifdef __cplusplus
}
#endif

#endif /* MCXN236_ADS1299_BOUNDED_STREAM_H */
