#include "ads1299.h"

#include <stdio.h>
#include <string.h>

static int failures = 0;

#define CHECK(expr) do { \
    if (!(expr)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #expr); \
        ++failures; \
    } \
} while (0)

static void test_frame_geometry(void) {
    CHECK(ads1299_frame_bytes_for_variant(ADS1299_VARIANT_4CH) == 15u);
    CHECK(ads1299_frame_bytes_for_variant(ADS1299_VARIANT_6CH) == 21u);
    CHECK(ads1299_frame_bytes_for_variant(ADS1299_VARIANT_8CH) == ADS1299_FRAME_BYTES);
    CHECK(ads1299_frame_bytes_for_variant(ADS1299_VARIANT_UNKNOWN) == 0u);
}

static void put_s24(uint8_t *dst, int32_t value) {
    const uint32_t u = (uint32_t)value & 0x00FFFFFFu;
    dst[0] = (uint8_t)(u >> 16);
    dst[1] = (uint8_t)(u >> 8);
    dst[2] = (uint8_t)u;
}

static void test_frame_decode_8ch(void) {
    static const int32_t expected[ADS1299_CHANNEL_COUNT] = {
        0, 1, -1, 8388607, -8388608, 0x123456, -0x123456, 42
    };
    uint8_t raw[ADS1299_FRAME_BYTES];
    ads1299_frame_t frame;
    ads1299_frame_status_t status;
    size_t ch;

    memset(raw, 0, sizeof(raw));
    raw[0] = 0xC0u; /* ADS1299 status header = 1100b. */
    raw[1] = 0x00u;
    raw[2] = 0x05u; /* GPIO nibble only; no lead-off bits set. */
    for (ch = 0u; ch < ADS1299_CHANNEL_COUNT; ++ch) {
        put_s24(&raw[ADS1299_STATUS_BYTES + ch * ADS1299_BYTES_PER_CHANNEL], expected[ch]);
    }

    CHECK(ads1299_decode_frame_variant(raw, sizeof(raw), ADS1299_VARIANT_8CH,
                                       &frame, &status) == ADS1299_OK);
    CHECK(status.header_valid == 1u);
    CHECK(status.header_nibble == 0x0Cu);
    CHECK(status.loff_statp == 0u);
    CHECK(status.loff_statn == 0u);
    CHECK(status.gpio_data == 0x05u);
    for (ch = 0u; ch < ADS1299_CHANNEL_COUNT; ++ch) {
        CHECK(frame.channel[ch] == expected[ch]);
    }

    raw[0] = 0xB0u;
    CHECK(ads1299_decode_frame_variant(raw, sizeof(raw), ADS1299_VARIANT_8CH,
                                       &frame, &status) == ADS1299_EVERIFY);
    raw[0] = 0xC0u;
    CHECK(ads1299_decode_frame_variant(raw, sizeof(raw) - 1u, ADS1299_VARIANT_8CH,
                                       &frame, &status) == ADS1299_EINVAL);
}

static void test_sign_extension(void) {
    CHECK(ads1299_sign_extend24(0x000000u) == 0);
    CHECK(ads1299_sign_extend24(0x000001u) == 1);
    CHECK(ads1299_sign_extend24(0x7FFFFFu) == 8388607);
    CHECK(ads1299_sign_extend24(0x800000u) == -8388608);
    CHECK(ads1299_sign_extend24(0xFFFFFFu) == -1);
}

static void test_register_safety_model(void) {
    uint8_t sanitized = 0u;

    CHECK(ads1299_register_write_value_valid(ADS1299_REG_CONFIG1,
                                              ADS1299_RESET_CONFIG1,
                                              ADS1299_VARIANT_8CH) == 1);
    CHECK(ads1299_register_write_value_valid(ADS1299_REG_ID,
                                              0x00u,
                                              ADS1299_VARIANT_8CH) == 0);
    CHECK(ads1299_register_write_value_valid(ADS1299_REG_CONFIG1,
                                              (uint8_t)(ADS1299_CONFIG1_RESERVED_BASE |
                                                        ADS1299_DR_RESERVED),
                                              ADS1299_VARIANT_8CH) == 0);
    CHECK(ads1299_sanitize_register_write(ADS1299_REG_CONFIG1,
                                           ADS1299_DR_250SPS,
                                           ADS1299_VARIANT_8CH,
                                           &sanitized) == 0);
    CHECK((sanitized & ADS1299_CONFIG1_DR_MASK) == ADS1299_DR_250SPS);
    CHECK((sanitized & 0x90u) == 0x90u);
}

static void test_semantics(void) {
    double value = 0.0;

    CHECK(ads1299_semantic_data_rate_sps(ADS1299_DR_250SPS, 2048000.0, &value)
          == ADS1299_SEMANTIC_OK);
    CHECK(value > 249.999 && value < 250.001);

    CHECK(ads1299_semantic_gain(ADS1299_GAIN_CODE_24, &value)
          == ADS1299_SEMANTIC_OK);
    CHECK(value == 24.0);

    CHECK(ads1299_semantic_data_rate_sps(ADS1299_DR_RESERVED, 2048000.0, &value)
          == ADS1299_SEMANTIC_EINVAL);
}

int main(void) {
    test_frame_geometry();
    test_frame_decode_8ch();
    test_sign_extension();
    test_register_safety_model();
    test_semantics();

    if (failures != 0) {
        fprintf(stderr, "ADS1299 core host self-test: %d failure(s)\n", failures);
        return 1;
    }

    puts("ADS1299 core host self-test: PASS");
    puts("Validated in software only: frame geometry/decode, 24-bit sign extension,");
    puts("register safety rules, 250-SPS semantics, and gain semantics.");
    puts("No real ADS1299 hardware, electrical, EMC, or medical validation is implied.");
    return 0;
}
