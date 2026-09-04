#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "ads1299_field_model.h"
#include "ads1299_runtime.h"

static void test_metadata_inventory(void) {
    assert(ads1299_field_count() == (size_t)ADS1299_FIELD_COUNT);
    for (int i = 0; i < ADS1299_FIELD_COUNT; ++i) {
        const ads1299_field_info_t *info = ads1299_field_info((ads1299_field_id_t)i);
        assert(info != NULL);
        assert(info->id == (ads1299_field_id_t)i);
        assert(info->name != NULL && info->name[0] != '\0');
        assert(info->register_address <= ADS1299_REG_LAST);
        assert(info->mask != 0u);
    }
    assert(ads1299_field_info(ADS1299_FIELD_COUNT) == NULL);
}

static void test_ti_enumerated_validity(void) {
    for (uint8_t dr = 0u; dr <= 6u; ++dr)
        assert(ads1299_field_code_valid(ADS1299_FIELD_CONFIG1_DR, dr,
                                        ADS1299_VARIANT_8CH));
    assert(!ads1299_field_code_valid(ADS1299_FIELD_CONFIG1_DR, 7u,
                                     ADS1299_VARIANT_8CH));

    assert(ads1299_field_code_valid(ADS1299_FIELD_CONFIG2_CAL_FREQ, 0u,
                                    ADS1299_VARIANT_8CH));
    assert(ads1299_field_code_valid(ADS1299_FIELD_CONFIG2_CAL_FREQ, 1u,
                                    ADS1299_VARIANT_8CH));
    assert(!ads1299_field_code_valid(ADS1299_FIELD_CONFIG2_CAL_FREQ, 2u,
                                     ADS1299_VARIANT_8CH));
    assert(ads1299_field_code_valid(ADS1299_FIELD_CONFIG2_CAL_FREQ, 3u,
                                    ADS1299_VARIANT_8CH));

    for (uint8_t gain = 0u; gain <= 6u; ++gain)
        assert(ads1299_field_code_valid(ADS1299_FIELD_CH_GAIN, gain,
                                        ADS1299_VARIANT_8CH));
    assert(!ads1299_field_code_valid(ADS1299_FIELD_CH_GAIN, 7u,
                                     ADS1299_VARIANT_8CH));
}

static void test_variant_mask_fields(void) {
    assert(ads1299_field_code_valid(ADS1299_FIELD_BIAS_SENSP_MASK, 0x0Fu,
                                    ADS1299_VARIANT_4CH));
    assert(!ads1299_field_code_valid(ADS1299_FIELD_BIAS_SENSP_MASK, 0x10u,
                                     ADS1299_VARIANT_4CH));
    assert(ads1299_field_code_valid(ADS1299_FIELD_LOFF_FLIP_MASK, 0x3Fu,
                                    ADS1299_VARIANT_6CH));
    assert(!ads1299_field_code_valid(ADS1299_FIELD_LOFF_FLIP_MASK, 0x40u,
                                     ADS1299_VARIANT_6CH));
}

static void test_channel_relative_addressing(void) {
    uint8_t address = 0u;
    assert(ads1299_field_register_address(ADS1299_FIELD_CH_GAIN, 4u,
                                          ADS1299_VARIANT_4CH, &address) == 0);
    assert(address == ADS1299_REG_CH4SET);
    assert(ads1299_field_register_address(ADS1299_FIELD_CH_GAIN, 5u,
                                          ADS1299_VARIANT_4CH, &address) == -1);
    assert(ads1299_field_register_address(ADS1299_FIELD_CONFIG1_DR, 0u,
                                          ADS1299_VARIANT_4CH, &address) == 0);
    assert(address == ADS1299_REG_CONFIG1);
}

static void test_decode_and_encode(void) {
    uint8_t code = 0u;
    assert(ads1299_field_decode(ADS1299_FIELD_CONFIG1_DR,
                                ADS1299_RESET_CONFIG1, &code) == 0);
    assert(code == ADS1299_DR_250SPS);

    uint8_t encoded = 0u;
    assert(ads1299_field_encode(ADS1299_FIELD_CONFIG1_DR, 0u,
                                ADS1299_DR_1KSPS, ADS1299_RESET_CONFIG1,
                                ADS1299_VARIANT_8CH, &encoded) == 0);
    assert(encoded == 0x94u);

    assert(ads1299_field_encode(ADS1299_FIELD_CONFIG1_DR, 0u, 7u,
                                ADS1299_RESET_CONFIG1,
                                ADS1299_VARIANT_8CH, &encoded) == -1);

    assert(ads1299_field_encode(ADS1299_FIELD_CH_GAIN, 3u, 5u,
                                ADS1299_RESET_CHNSET,
                                ADS1299_VARIANT_8CH, &encoded) == 0);
    assert((encoded & ADS1299_CH_GAIN_MASK) == ADS1299_GAIN_12);

    /* BIAS_STAT is read-only. A high status bit from RREG must never be echoed
     * into the WREG byte while changing another CONFIG3 field. */
    assert(ads1299_field_encode(ADS1299_FIELD_CONFIG3_PD_BIAS, 0u, 1u,
                                (uint8_t)(ADS1299_RESET_CONFIG3 |
                                          ADS1299_CONFIG3_BIAS_STAT),
                                ADS1299_VARIANT_8CH, &encoded) == 0);
    assert((encoded & ADS1299_CONFIG3_BIAS_STAT) == 0u);
    assert((encoded & ADS1299_CONFIG3_PD_BIAS) != 0u);

    assert(ads1299_field_encode(ADS1299_FIELD_CONFIG3_BIAS_STAT, 0u, 1u,
                                ADS1299_RESET_CONFIG3,
                                ADS1299_VARIANT_8CH, &encoded) == -1);
}

typedef struct {
    uint8_t rx_fill;
    uint8_t tx_first[16];
    size_t calls;
} mock_t;

static int spi_transfer(void *user, const uint8_t *tx, uint8_t *rx, size_t len) {
    mock_t *m = (mock_t *)user;
    if (tx && len && m->calls < 16u) m->tx_first[m->calls] = tx[0];
    if (rx) memset(rx, m->rx_fill, len);
    m->calls++;
    return 0;
}
static void gpio_write(void *user, int level) { (void)user; (void)level; }
static void delay_us(void *user, uint32_t us) { (void)user; (void)us; }

static void test_safe_field_write_rejects_before_wreg(void) {
    mock_t mock = {.rx_fill = ADS1299_RESET_CONFIG1};
    ads1299_port_t port = {
        .user = &mock, .spi_transfer = spi_transfer, .cs_write = gpio_write,
        .delay_us = delay_us
    };
    ads1299_t dev;
    assert(ads1299_init(&dev, &port) == ADS1299_OK);

    size_t before = mock.calls;
    assert(ads1299_safe_write_field(&dev, ADS1299_FIELD_CONFIG1_DR, 0u, 7u,
                                    ADS1299_VARIANT_8CH, NULL) == ADS1299_EINVAL);
    assert(mock.calls == before);

    uint8_t written = 0u;
    assert(ads1299_safe_write_field(&dev, ADS1299_FIELD_CONFIG1_DR, 0u,
                                    ADS1299_DR_1KSPS, ADS1299_VARIANT_8CH,
                                    &written) == ADS1299_OK);
    assert((written & ADS1299_CONFIG1_DR_MASK) == ADS1299_DR_1KSPS);
}

int main(void) {
    test_metadata_inventory();
    test_ti_enumerated_validity();
    test_variant_mask_fields();
    test_channel_relative_addressing();
    test_decode_and_encode();
    test_safe_field_write_rejects_before_wreg();
    puts("ADS1299 field-model tests passed");
    return 0;
}
