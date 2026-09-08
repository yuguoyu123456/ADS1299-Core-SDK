#include "../ads1299_packet.h"

#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static void test_sign_extension(void) {
    const uint8_t zero[3] = {0x00, 0x00, 0x00};
    const uint8_t maxp[3] = {0x7F, 0xFF, 0xFF};
    const uint8_t minm[3] = {0x80, 0x00, 0x00};
    const uint8_t neg1[3] = {0xFF, 0xFF, 0xFF};

    assert(ads1299_decode_sample24_be(zero) == 0);
    assert(ads1299_decode_sample24_be(maxp) == 8388607);
    assert(ads1299_decode_sample24_be(minm) == -8388608);
    assert(ads1299_decode_sample24_be(neg1) == -1);
}

static void test_raw_frame(void) {
    uint8_t raw[27] = {0};
    uint8_t status[3] = {0};
    int32_t channels[8] = {0};

    raw[0] = 0xC0;
    raw[1] = 0x12;
    raw[2] = 0x34;

    for (unsigned ch = 0; ch < 8; ++ch) {
        int32_t value = (int32_t)(1000 * (int)(ch + 1));
        ads1299_encode_sample24_be(value, &raw[3 + ch * 3]);
    }

    assert(ads1299_decode_raw_frame(raw, sizeof(raw), status, channels));
    assert(status[0] == 0xC0 && status[1] == 0x12 && status[2] == 0x34);
    for (unsigned ch = 0; ch < 8; ++ch) {
        assert(channels[ch] == (int32_t)(1000 * (int)(ch + 1)));
    }
}

static void test_packet_round_trip(void) {
    const uint8_t status[3] = {0xC0, 0x00, 0x01};
    const int32_t channels[8] = {
        1, -1, 8388607, -8388608, 123456, -654321, 0, 42
    };
    uint8_t wire[ADS1299_PACKET_SIZE];
    ads1299_packet_t decoded;

    const size_t n = ads1299_packet_encode(
        wire,
        sizeof(wire),
        ADS1299_PACKET_FLAG_DATA_VALID | ADS1299_PACKET_FLAG_TIMESTAMP_VALID,
        1234u,
        987654u,
        status,
        channels);

    assert(n == ADS1299_PACKET_SIZE);
    assert(ads1299_packet_validate(wire, sizeof(wire)) == ADS1299_PACKET_OK);
    assert(ads1299_packet_decode(wire, sizeof(wire), &decoded) == ADS1299_PACKET_OK);
    assert(decoded.sequence == 1234u);
    assert(decoded.timestamp_us == 987654u);
    assert(memcmp(decoded.status, status, sizeof(status)) == 0);
    assert(memcmp(decoded.channels, channels, sizeof(channels)) == 0);

    wire[20] ^= 0x01u;
    assert(ads1299_packet_validate(wire, sizeof(wire)) == ADS1299_PACKET_ERR_CRC);
}

static void test_scaling(void) {
    const double uv = ads1299_code_to_microvolts(8388607, 4.5, 24);
    assert(fabs(uv - 187500.0) < 0.01);
}

int main(void) {
    test_sign_extension();
    test_raw_frame();
    test_packet_round_trip();
    test_scaling();
    puts("ads1299_packet tests: PASS");
    return 0;
}
