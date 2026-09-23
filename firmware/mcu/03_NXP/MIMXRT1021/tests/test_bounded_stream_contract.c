/* Host/smoke contract test for the public bounded-stream handoff API.
 * This intentionally tests only the platform-facing type/signature contract;
 * queue scheduling and hardware acquisition remain target integration tests.
 */
#include <stdint.h>
#include "../examples/bounded_stream.h"

_Static_assert(sizeof(((ads1299_stream_item_t *)0)->sequence) == sizeof(uint32_t),
               "stream sequence must remain uint32_t");
_Static_assert(sizeof(((ads1299_stream_item_t *)0)->frame) == sizeof(ads1299_frame_t),
               "stream payload must remain the shared-core ads1299_frame_t");

static int (*const stream_pop_contract)(ads1299_stream_item_t *) = ads1299_stream_try_pop;
static volatile uint32_t *const stream_produced_contract = &ads1299_stream_produced;
static volatile uint32_t *const stream_dropped_contract = &ads1299_stream_dropped;

int main(void)
{
    ads1299_stream_item_t item = {0};

    /* Keep the public symbols type-checked without requiring a target HAL or
     * invoking the queue implementation in this smoke translation unit. */
    (void)item;
    (void)stream_pop_contract;
    (void)stream_produced_contract;
    (void)stream_dropped_contract;
    return 0;
}
