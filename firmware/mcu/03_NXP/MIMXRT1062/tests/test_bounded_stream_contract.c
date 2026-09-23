#include <stdint.h>
#include "../examples/bounded_stream.h"

/* Host/compile smoke contract for the transport-neutral streaming handoff.
 * This intentionally does not require MCUXpresso headers or hardware. */
_Static_assert(sizeof(((ads1299_stream_item_t *)0)->sequence) == sizeof(uint32_t),
               "stream sequence must remain uint32_t");
_Static_assert(sizeof(((ads1299_stream_item_t *)0)->frame.status) == 3u,
               "shared ADS1299 frame must expose 3 status bytes");
_Static_assert((sizeof(((ads1299_stream_item_t *)0)->frame.channel) /
                sizeof(((ads1299_stream_item_t *)0)->frame.channel[0])) == 8u,
               "shared ADS1299 frame must expose 8 channels");

static int (*const stream_pop_contract)(ads1299_stream_item_t *) =
    ads1299_stream_try_pop;
static volatile uint32_t *const produced_contract = &ads1299_stream_produced;
static volatile uint32_t *const dropped_contract = &ads1299_stream_dropped;

int main(void)
{
    /* Keep symbols referenced so -Wall/-Wextra builds also exercise the API. */
    return (stream_pop_contract != 0 &&
            produced_contract != 0 &&
            dropped_contract != 0) ? 0 : 1;
}
