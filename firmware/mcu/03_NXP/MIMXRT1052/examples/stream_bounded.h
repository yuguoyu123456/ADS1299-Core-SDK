#ifndef MIMXRT1052_ADS1299_STREAM_BOUNDED_H
#define MIMXRT1052_ADS1299_STREAM_BOUNDED_H

#include <stdint.h>
#include "ads1299.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Public transport-side contract for the bounded streaming example.
 * Acquisition owns queue production; foreground/RTOS transport code consumes
 * complete shared-core frames without knowing ADS1299 register details.
 */
int ads1299_stream_try_pop(ads1299_frame_t *frame);

extern volatile uint32_t ads1299_stream_acquired;
extern volatile uint32_t ads1299_stream_sent;
extern volatile uint32_t ads1299_stream_dropped;
extern volatile uint32_t ads1299_stream_transport_errors;

#ifdef __cplusplus
}
#endif

#endif /* MIMXRT1052_ADS1299_STREAM_BOUNDED_H */
