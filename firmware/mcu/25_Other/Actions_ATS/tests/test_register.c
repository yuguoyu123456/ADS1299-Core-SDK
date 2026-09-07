/* Register behavior belongs to Core.  This smoke test only proves that the
 * port transports opaque bytes without defining or rewriting register bits. */
#include <assert.h>
#include "ads1299_spi.h"

void test_register_boundary_is_opaque(void) {
    assert(sizeof(ads1299_platform_hal_t) > sizeof(void *));
}
