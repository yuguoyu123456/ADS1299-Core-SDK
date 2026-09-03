#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include "ads1299.h"

int main(void) {
    assert(ads1299_sign_extend24(0x000000u) == 0);
    assert(ads1299_sign_extend24(0x000001u) == 1);
    assert(ads1299_sign_extend24(0x7FFFFFu) == 8388607);
    assert(ads1299_sign_extend24(0x800000u) == -8388608);
    assert(ads1299_sign_extend24(0xFFFFFFu) == -1);

    double v = ads1299_code_to_volts(8388607, 4.5, 24.0);
    assert(v > 0.18749 && v < 0.18751);

    puts("ADS1299 core math tests passed");
    return 0;
}
