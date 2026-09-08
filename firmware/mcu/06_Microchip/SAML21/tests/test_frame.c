#include <assert.h>
#include "ads1299.h"

void test_core_frame_numeric_boundary(void) {
    assert(ads1299_sign_extend24(0x000001u) == 1);
    assert(ads1299_sign_extend24(0xFFFFFFu) == -1);
    assert(ads1299_sign_extend24(0x800000u) == -8388608);
}
