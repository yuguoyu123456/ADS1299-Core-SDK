#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "ads1299.h"

typedef struct {
    size_t data_length;
} frame_mock_t;

static int frame_spi(void *user, const uint8_t *tx, uint8_t *rx, size_t len) {
    frame_mock_t *mock = (frame_mock_t *)user;
    (void)tx;
    if (len > 1u) {
        mock->data_length = len;
        memset(rx, 0, len);
        rx[0] = 0xC0u;
    }
    return 0;
}
static void frame_gpio(void *user, int level) { (void)user; (void)level; }
static void frame_delay(void *user, uint32_t us) { (void)user; (void)us; }

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

static void test_device_acquisition_uses_detected_length(void) {
    frame_mock_t mock = {0};
    ads1299_port_t port = {
        .user = &mock,
        .spi_transfer = frame_spi,
        .cs_write = frame_gpio,
        .reset_write = frame_gpio,
        .start_write = frame_gpio,
        .delay_us = frame_delay,
    };
    ads1299_t dev;
    ads1299_frame_t frame;
    assert(ads1299_init(&dev, &port) == ADS1299_OK);
    dev.channel_count = 4u;
    dev.continuous_mode = 1u;
    assert(ads1299_read_frame_continuous(&dev, &frame) == ADS1299_OK);
    assert(mock.data_length == 15u);
    for (size_t i = 4u; i < 8u; ++i) assert(frame.channel[i] == 0);

    mock.data_length = 0u;
    dev.channel_count = 6u;
    assert(ads1299_read_frame_rdata(&dev, &frame) == ADS1299_OK);
    assert(mock.data_length == 21u);
}

int main(void) {
    test_lengths();
    test_status_decode();
    test_bad_header();
    test_variant_frame_decode();
    test_device_acquisition_uses_detected_length();
    puts("ADS1299 variant frame/status tests passed");
    return 0;
}
