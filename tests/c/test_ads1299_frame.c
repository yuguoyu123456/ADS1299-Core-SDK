#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "ads1299_frame.h"

static void test_lengths(void) {
    assert(ads1299_frame_bytes_for_variant(ADS1299_VARIANT_4CH) == 15u);
    assert(ads1299_frame_bytes_for_variant(ADS1299_VARIANT_6CH) == 21u);
    assert(ads1299_frame_bytes_for_variant(ADS1299_VARIANT_8CH) == 27u);
    assert(ads1299_frame_bytes_for_variant(ADS1299_VARIANT_UNKNOWN) == 0u);
}

static void test_status_decode(void) {
    /* 1100 + LOFF_STATP(0xA5) + LOFF_STATN(0x3C) + GPIO(0x9). */
    const uint8_t raw[3] = {0xCAu, 0x53u, 0xC9u};
    ads1299_frame_status_t s;
    assert(ads1299_decode_frame_status(raw, ADS1299_VARIANT_8CH, &s) == ADS1299_OK);
    assert(s.header_valid == 1u);
    assert(s.header_nibble == 0x0Cu);
    assert(s.loff_statp == 0xA5u);
    assert(s.loff_statn == 0x3Cu);
    assert(s.gpio_data == 0x09u);

    assert(ads1299_decode_frame_status(raw, ADS1299_VARIANT_4CH, &s) == ADS1299_OK);
    assert(s.loff_statp == 0x05u);
    assert(s.loff_statn == 0x0Cu);
}

static void test_bad_header(void) {
    const uint8_t raw[3] = {0xBAu, 0x53u, 0xC9u};
    ads1299_frame_status_t s;
    assert(ads1299_decode_frame_status(raw, ADS1299_VARIANT_8CH, &s) == ADS1299_EVERIFY);
    assert(s.header_valid == 0u);
}

static void test_variant_frame_decode(void) {
    uint8_t raw[27] = {0};
    raw[0] = 0xC0u;
    raw[1] = 0x00u;
    raw[2] = 0x00u;
    /* CH1 = +1, CH2 = -1, CH3 = +FS, CH4 = -FS. */
    raw[5] = 0x01u;
    raw[6] = 0xFFu; raw[7] = 0xFFu; raw[8] = 0xFFu;
    raw[9] = 0x7Fu; raw[10] = 0xFFu; raw[11] = 0xFFu;
    raw[12] = 0x80u;

    ads1299_frame_t frame;
    ads1299_frame_status_t status;
    assert(ads1299_decode_frame_variant(raw, 15u, ADS1299_VARIANT_4CH,
                                        &frame, &status) == ADS1299_OK);
    assert(frame.channel[0] == 1);
    assert(frame.channel[1] == -1);
    assert(frame.channel[2] == 8388607);
    assert(frame.channel[3] == -8388608);
    for (size_t i = 4u; i < 8u; ++i) assert(frame.channel[i] == 0);

    assert(ads1299_decode_frame_variant(raw, 14u, ADS1299_VARIANT_4CH,
                                        &frame, &status) == ADS1299_EINVAL);
}

int main(void) {
    test_lengths();
    test_status_decode();
    test_bad_header();
    test_variant_frame_decode();
    puts("ADS1299 variant frame/status tests passed");
    return 0;
}
