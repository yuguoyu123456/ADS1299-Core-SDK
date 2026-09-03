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

    /* TI CONFIG2 internal-test encoding: reserved base 0xC0 + INT_CAL 0x10. */
    assert(ads1299_make_internal_test_config2(0, ADS1299_TEST_FREQ_FCLK_2_21) == 0xD0u);
    assert(ads1299_make_internal_test_config2(1, ADS1299_TEST_FREQ_FCLK_2_20) == 0xD5u);
    assert(ads1299_make_internal_test_config2(0, ADS1299_TEST_FREQ_DC) == 0xD3u);

    puts("ADS1299 core math/config tests passed");
    return 0;
}
