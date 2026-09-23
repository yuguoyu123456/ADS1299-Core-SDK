#include <stdint.h>
#include "../examples/stream_bounded.h"

/*
 * Compile-only smoke test for the transport-side bounded-stream contract.
 * This deliberately does not require MCUXpresso or hardware: it catches
 * accidental API drift while keeping the platform example dependent on the
 * shared ADS1299 frame type.
 */

_Static_assert(sizeof(((ads1299_frame_t *)0)->status) == 3u,
               "ADS1299 frame status must remain three bytes");
_Static_assert(sizeof(((ads1299_frame_t *)0)->ch) / sizeof(((ads1299_frame_t *)0)->ch[0]) == 8u,
               "ADS1299-8 frame must expose eight channels");

static int (*const stream_pop_contract)(ads1299_frame_t *) = ads1299_stream_try_pop;
static volatile uint32_t *const acquired_contract = &ads1299_stream_acquired;
static volatile uint32_t *const sent_contract = &ads1299_stream_sent;
static volatile uint32_t *const dropped_contract = &ads1299_stream_dropped;
static volatile uint32_t *const transport_error_contract = &ads1299_stream_transport_errors;

int main(void)
{
    return (stream_pop_contract != 0 &&
            acquired_contract != 0 &&
            sent_contract != 0 &&
            dropped_contract != 0 &&
            transport_error_contract != 0) ? 0 : 1;
}
