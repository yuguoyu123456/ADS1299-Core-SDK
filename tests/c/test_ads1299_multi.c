#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "ads1299.h"

static void test_mixed_variant_daisy_decode(void) {
    const ads1299_variant_t variants[2] = {
        ADS1299_VARIANT_4CH, ADS1299_VARIANT_6CH
    };
    uint8_t raw[36] = {0};
    raw[0] = 0xC0u;
    raw[5] = 0x01u; /* device 1, CH1 = +1 */
    raw[15] = 0xC0u;
    raw[20] = 0x02u; /* device 2, CH1 = +2 */

    ads1299_chain_frame_t frames[2];
    assert(ads1299_daisy_frame_bytes(variants, 2u) == sizeof(raw));
    assert(ads1299_decode_daisy_frame(raw, sizeof(raw), variants, 2u, frames) ==
           ADS1299_OK);
    assert(frames[0].variant == ADS1299_VARIANT_4CH);
    assert(frames[0].frame.channel[0] == 1);
    assert(frames[0].frame.channel[4] == 0);
    assert(frames[1].variant == ADS1299_VARIANT_6CH);
    assert(frames[1].frame.channel[0] == 2);
    assert(frames[1].frame.channel[6] == 0);
}

static void test_invalid_chain_rejected(void) {
    ads1299_variant_t bad[1] = {ADS1299_VARIANT_UNKNOWN};
    ads1299_chain_frame_t frame;
    uint8_t raw[3] = {0xC0u, 0u, 0u};
    assert(ads1299_daisy_frame_bytes(bad, 1u) == 0u);
    assert(ads1299_decode_daisy_frame(raw, sizeof(raw), bad, 1u, &frame) ==
           ADS1299_EINVAL);
}

int main(void) {
    test_mixed_variant_daisy_decode();
    test_invalid_chain_rejected();
    puts("ADS1299 multi-device daisy tests passed");
    return 0;
}
