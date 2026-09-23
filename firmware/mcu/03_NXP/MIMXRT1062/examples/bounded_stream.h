#ifndef MIMXRT1062_ADS1299_BOUNDED_STREAM_H
#define MIMXRT1062_ADS1299_BOUNDED_STREAM_H

#include <stdint.h>
#include "ads1299.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Transport-neutral handoff item produced by the 250-SPS acquisition path.
 * The frame remains the shared-core ADS1299 representation; platform code must
 * not duplicate register or wire-frame parsing here. */
typedef struct {
    ads1299_frame_t frame;
    uint32_t sequence;
} ads1299_stream_item_t;

/* Non-blocking foreground consumer. Returns 1 when an item was copied to out,
 * otherwise 0 (empty queue or NULL output). Never call blocking transport from
 * the DRDY/acquisition path. */
int ads1299_stream_try_pop(ads1299_stream_item_t *out);

/* Monotonic producer/drop counters for diagnostics and transport health. */
extern volatile uint32_t ads1299_stream_produced;
extern volatile uint32_t ads1299_stream_dropped;

#ifdef __cplusplus
}
#endif

#endif /* MIMXRT1062_ADS1299_BOUNDED_STREAM_H */
