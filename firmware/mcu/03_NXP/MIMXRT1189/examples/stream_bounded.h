#ifndef ADS1299_MIMXRT1189_STREAM_BOUNDED_H
#define ADS1299_MIMXRT1189_STREAM_BOUNDED_H

#include <stdint.h>
#include "ads1299.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Public consumer contract for examples/stream_bounded.c.
 *
 * Acquisition owns the producer side of the bounded queue. A UART/USB/
 * Ethernet task may include this header and call ads1299_stream_pop() from
 * its non-DRDY context. This keeps slow transport work out of the ADS1299
 * acquisition timing path.
 */
typedef struct {
    ads1299_frame_t frame;
    uint32_t sequence;
} ads1299_stream_item_t;

/* Returns 1 and copies the oldest queued frame into out, 0 when empty,
 * or -1 for an invalid output pointer.
 */
int ads1299_stream_pop(ads1299_stream_item_t *out);

/* Diagnostics exported by the example. overflow_count increments when the
 * fixed queue is full and the newest frame is intentionally dropped.
 */
extern volatile int ads1299_stream_result;
extern volatile uint32_t ads1299_stream_overflow_count;
extern volatile uint32_t ads1299_stream_frame_count;

#ifdef __cplusplus
}
#endif

#endif /* ADS1299_MIMXRT1189_STREAM_BOUNDED_H */
