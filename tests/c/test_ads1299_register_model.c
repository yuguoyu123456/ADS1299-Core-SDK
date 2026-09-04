#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "ads1299_register_model.h"

static void test_complete_address_coverage(void) {
    for (uint8_t address = ADS1299_REG_ID; address <= ADS1299_REG_LAST; ++address) {
        const ads1299_register_info_t *info = ads1299_register_info(address);
        assert(info != NULL);
        assert(info->address == address);
    }
    assert(ads1299_register_info((uint8_t)(ADS1299_REG_LAST + 1u)) == NULL);
}

static void test_reset_values(void) {
    const uint8_t expected[ADS1299_REGISTER_COUNT] = {
        0x00u, ADS1299_RESET_CONFIG1, ADS1299_RESET_CONFIG2, ADS1299_RESET_CONFIG3,
        ADS1299_RESET_LOFF,
        ADS1299_RESET_CHNSET, ADS1299_RESET_CHNSET, ADS1299_RESET_CHNSET,
        ADS1299_RESET_CHNSET, ADS1299_RESET_CHNSET, ADS1299_RESET_CHNSET,
        ADS1299_RESET_CHNSET, ADS1299_RESET_CHNSET,
        ADS1299_RESET_BIAS_SENSP, ADS1299_RESET_BIAS_SENSN,
        ADS1299_RESET_LOFF_SENSP, ADS1299_RESET_LOFF_SENSN,
        ADS1299_RESET_LOFF_FLIP, ADS1299_RESET_LOFF_STATP,
        ADS1299_RESET_LOFF_STATN, ADS1299_RESET_GPIO, ADS1299_RESET_MISC1,
        ADS1299_RESET_MISC2, ADS1299_RESET_CONFIG4
    };
    for (uint8_t address = ADS1299_REG_CONFIG1; address <= ADS1299_REG_LAST; ++address) {
        const ads1299_register_info_t *info = ads1299_register_info(address);
        assert(info->reset_known == 1u);
        assert(info->reset_value == expected[address]);
    }
    assert(ads1299_register_info(ADS1299_REG_ID)->reset_known == 0u);
}

static void test_read_only_and_reserved_registers(void) {
    uint8_t value = 0xAAu;
    assert(ads1299_register_info(ADS1299_REG_ID)->read_only == 1u);
    assert(ads1299_register_info(ADS1299_REG_LOFF_STATP)->read_only == 1u);
    assert(ads1299_register_info(ADS1299_REG_LOFF_STATN)->read_only == 1u);
    assert(ads1299_sanitize_register_write(ADS1299_REG_ID, 0xFFu,
                                           ADS1299_VARIANT_8CH, &value) == -1);
    assert(ads1299_sanitize_register_write(ADS1299_REG_MISC2, 0xFFu,
                                           ADS1299_VARIANT_8CH, &value) == 0);
    assert(value == 0x00u);
}

static void test_required_reserved_bits(void) {
    uint8_t value = 0u;
    assert(ads1299_sanitize_register_write(ADS1299_REG_CONFIG1, 0x00u,
                                           ADS1299_VARIANT_8CH, &value) == 0);
    assert(value == 0x90u);
    assert(ads1299_sanitize_register_write(ADS1299_REG_CONFIG1, 0xF6u,
                                           ADS1299_VARIANT_8CH, &value) == 0);
    assert(value == 0xF6u);
    assert(ads1299_sanitize_register_write(ADS1299_REG_CONFIG2, 0xD5u,
                                           ADS1299_VARIANT_8CH, &value) == 0);
    assert(value == 0xD5u);
    assert(ads1299_sanitize_register_write(ADS1299_REG_CONFIG3, 0x00u,
                                           ADS1299_VARIANT_8CH, &value) == 0);
    assert(value == 0x60u);
    assert(ads1299_sanitize_register_write(ADS1299_REG_LOFF, 0xFFu,
                                           ADS1299_VARIANT_8CH, &value) == 0);
    assert(value == 0xEFu);
    assert(ads1299_sanitize_register_write(ADS1299_REG_MISC1, 0xFFu,
                                           ADS1299_VARIANT_8CH, &value) == 0);
    assert(value == ADS1299_MISC1_SRB1);
    assert(ads1299_sanitize_register_write(ADS1299_REG_CONFIG4, 0xFFu,
                                           ADS1299_VARIANT_8CH, &value) == 0);
    assert(value == 0x0Au);
}

static void test_forbidden_field_encodings(void) {
    uint8_t value = 0u;

    /* TI CONFIG1 DR=111 is reserved. */
    assert(!ads1299_register_write_value_valid(ADS1299_REG_CONFIG1, 0x97u,
                                                ADS1299_VARIANT_8CH));
    assert(ads1299_sanitize_register_write(ADS1299_REG_CONFIG1, 0xFFu,
                                           ADS1299_VARIANT_8CH, &value) == -1);
    assert(ads1299_register_write_value_valid(ADS1299_REG_CONFIG1, 0x96u,
                                               ADS1299_VARIANT_8CH));

    /* TI CONFIG2 CAL_FREQ=10 is explicitly Do not use. */
    assert(!ads1299_register_write_value_valid(ADS1299_REG_CONFIG2, 0xC2u,
                                                ADS1299_VARIANT_8CH));
    assert(ads1299_sanitize_register_write(ADS1299_REG_CONFIG2, 0xC2u,
                                           ADS1299_VARIANT_8CH, &value) == -1);
    assert(ads1299_register_write_value_valid(ADS1299_REG_CONFIG2, 0xD5u,
                                               ADS1299_VARIANT_8CH));

    /* TI CHnSET GAIN=111 is explicitly Do not use. */
    assert(!ads1299_register_write_value_valid(ADS1299_REG_CH1SET, 0x70u,
                                                ADS1299_VARIANT_8CH));
    assert(ads1299_sanitize_register_write(ADS1299_REG_CH1SET, 0x70u,
                                           ADS1299_VARIANT_8CH, &value) == -1);
    assert(ads1299_register_write_value_valid(ADS1299_REG_CH1SET, 0x60u,
                                               ADS1299_VARIANT_8CH));
}

static void test_exact_write_value_validation(void) {
    /* Prescribed reserved bits are part of exact-byte validation. */
    assert(!ads1299_register_write_value_valid(ADS1299_REG_CONFIG1, 0x06u,
                                                ADS1299_VARIANT_8CH));
    assert(ads1299_register_write_value_valid(ADS1299_REG_CONFIG1, 0x96u,
                                               ADS1299_VARIANT_8CH));
    assert(!ads1299_register_write_value_valid(ADS1299_REG_MISC1, 0x21u,
                                                ADS1299_VARIANT_8CH));
    assert(ads1299_register_write_value_valid(ADS1299_REG_MISC1, 0x20u,
                                               ADS1299_VARIANT_8CH));
}

static void test_variant_availability_and_masks(void) {
    uint8_t value = 0u;
    assert(ads1299_variant_channel_mask(ADS1299_VARIANT_4CH) == 0x0Fu);
    assert(ads1299_variant_channel_mask(ADS1299_VARIANT_6CH) == 0x3Fu);
    assert(ads1299_variant_channel_mask(ADS1299_VARIANT_8CH) == 0xFFu);
    assert(ads1299_variant_channel_mask(ADS1299_VARIANT_UNKNOWN) == 0x00u);
    assert(!ads1299_register_available(ADS1299_REG_CH5SET, ADS1299_VARIANT_4CH));
    assert(ads1299_register_available(ADS1299_REG_CH5SET, ADS1299_VARIANT_6CH));
    assert(!ads1299_register_available(ADS1299_REG_CH7SET, ADS1299_VARIANT_6CH));
    assert(ads1299_register_available(ADS1299_REG_CH7SET, ADS1299_VARIANT_8CH));
    assert(ads1299_sanitize_register_write(ADS1299_REG_BIAS_SENSP, 0xFFu,
                                           ADS1299_VARIANT_4CH, &value) == 0);
    assert(value == 0x0Fu);
    assert(ads1299_sanitize_register_write(ADS1299_REG_LOFF_SENSN, 0xFFu,
                                           ADS1299_VARIANT_6CH, &value) == 0);
    assert(value == 0x3Fu);
    assert(ads1299_sanitize_register_write(ADS1299_REG_LOFF_FLIP, 0xFFu,
                                           ADS1299_VARIANT_8CH, &value) == 0);
    assert(value == 0xFFu);
    assert(ads1299_sanitize_register_write(ADS1299_REG_CH8SET, 0x61u,
                                           ADS1299_VARIANT_6CH, &value) == -1);
    assert(!ads1299_register_write_value_valid(ADS1299_REG_BIAS_SENSP, 0x10u,
                                                ADS1299_VARIANT_4CH));
}

int main(void) {
    test_complete_address_coverage();
    test_reset_values();
    test_read_only_and_reserved_registers();
    test_required_reserved_bits();
    test_forbidden_field_encodings();
    test_exact_write_value_validation();
    test_variant_availability_and_masks();
    puts("ADS1299 complete register-model and field-semantics tests passed");
    return 0;
}
