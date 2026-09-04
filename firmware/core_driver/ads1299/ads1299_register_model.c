#include "ads1299_register_model.h"

#include <stddef.h>

#define REG_INFO(addr, reset, known, writable, ones, zeros, ro) \
    { (addr), (reset), (known), (writable), (ones), (zeros), (ro) }

static const ads1299_register_info_t k_registers[ADS1299_REGISTER_COUNT] = {
    REG_INFO(ADS1299_REG_ID,         0x00u,                    0u, 0x00u, 0x00u, 0x00u, 1u),
    REG_INFO(ADS1299_REG_CONFIG1,    ADS1299_RESET_CONFIG1,    1u, 0x67u, 0x90u, 0x08u, 0u),
    REG_INFO(ADS1299_REG_CONFIG2,    ADS1299_RESET_CONFIG2,    1u, 0x17u, 0xC0u, 0x28u, 0u),
    REG_INFO(ADS1299_REG_CONFIG3,    ADS1299_RESET_CONFIG3,    1u, 0x9Eu, 0x60u, 0x00u, 0u),
    REG_INFO(ADS1299_REG_LOFF,       ADS1299_RESET_LOFF,       1u, 0xEFu, 0x00u, 0x10u, 0u),
    REG_INFO(ADS1299_REG_CH1SET,     ADS1299_RESET_CHNSET,     1u, 0xFFu, 0x00u, 0x00u, 0u),
    REG_INFO(ADS1299_REG_CH2SET,     ADS1299_RESET_CHNSET,     1u, 0xFFu, 0x00u, 0x00u, 0u),
    REG_INFO(ADS1299_REG_CH3SET,     ADS1299_RESET_CHNSET,     1u, 0xFFu, 0x00u, 0x00u, 0u),
    REG_INFO(ADS1299_REG_CH4SET,     ADS1299_RESET_CHNSET,     1u, 0xFFu, 0x00u, 0x00u, 0u),
    REG_INFO(ADS1299_REG_CH5SET,     ADS1299_RESET_CHNSET,     1u, 0xFFu, 0x00u, 0x00u, 0u),
    REG_INFO(ADS1299_REG_CH6SET,     ADS1299_RESET_CHNSET,     1u, 0xFFu, 0x00u, 0x00u, 0u),
    REG_INFO(ADS1299_REG_CH7SET,     ADS1299_RESET_CHNSET,     1u, 0xFFu, 0x00u, 0x00u, 0u),
    REG_INFO(ADS1299_REG_CH8SET,     ADS1299_RESET_CHNSET,     1u, 0xFFu, 0x00u, 0x00u, 0u),
    REG_INFO(ADS1299_REG_BIAS_SENSP, ADS1299_RESET_BIAS_SENSP, 1u, 0xFFu, 0x00u, 0x00u, 0u),
    REG_INFO(ADS1299_REG_BIAS_SENSN, ADS1299_RESET_BIAS_SENSN, 1u, 0xFFu, 0x00u, 0x00u, 0u),
    REG_INFO(ADS1299_REG_LOFF_SENSP, ADS1299_RESET_LOFF_SENSP, 1u, 0xFFu, 0x00u, 0x00u, 0u),
    REG_INFO(ADS1299_REG_LOFF_SENSN, ADS1299_RESET_LOFF_SENSN, 1u, 0xFFu, 0x00u, 0x00u, 0u),
    REG_INFO(ADS1299_REG_LOFF_FLIP,  ADS1299_RESET_LOFF_FLIP,  1u, 0xFFu, 0x00u, 0x00u, 0u),
    REG_INFO(ADS1299_REG_LOFF_STATP, ADS1299_RESET_LOFF_STATP, 1u, 0x00u, 0x00u, 0x00u, 1u),
    REG_INFO(ADS1299_REG_LOFF_STATN, ADS1299_RESET_LOFF_STATN, 1u, 0x00u, 0x00u, 0x00u, 1u),
    REG_INFO(ADS1299_REG_GPIO,       ADS1299_RESET_GPIO,       1u, 0xFFu, 0x00u, 0x00u, 0u),
    REG_INFO(ADS1299_REG_MISC1,      ADS1299_RESET_MISC1,      1u, 0x20u, 0x00u, 0xDFu, 0u),
    REG_INFO(ADS1299_REG_MISC2,      ADS1299_RESET_MISC2,      1u, 0x00u, 0x00u, 0xFFu, 0u),
    REG_INFO(ADS1299_REG_CONFIG4,    ADS1299_RESET_CONFIG4,    1u, 0x0Au, 0x00u, 0xF5u, 0u),
};

const ads1299_register_info_t *ads1299_register_info(uint8_t address) {
    if (address > ADS1299_REG_LAST) return NULL;
    return &k_registers[address];
}

uint8_t ads1299_variant_channel_mask(ads1299_variant_t variant) {
    switch (variant) {
        case ADS1299_VARIANT_4CH: return 0x0Fu;
        case ADS1299_VARIANT_6CH: return 0x3Fu;
        case ADS1299_VARIANT_8CH: return 0xFFu;
        default: return 0x00u;
    }
}

int ads1299_register_available(uint8_t address, ads1299_variant_t variant) {
    if (address > ADS1299_REG_LAST || ads1299_variant_channel_mask(variant) == 0u) {
        return 0;
    }
    if (address == ADS1299_REG_CH5SET || address == ADS1299_REG_CH6SET) {
        return variant == ADS1299_VARIANT_6CH || variant == ADS1299_VARIANT_8CH;
    }
    if (address == ADS1299_REG_CH7SET || address == ADS1299_REG_CH8SET) {
        return variant == ADS1299_VARIANT_8CH;
    }
    return 1;
}

static int is_channel_mask_register(uint8_t address) {
    return address == ADS1299_REG_BIAS_SENSP || address == ADS1299_REG_BIAS_SENSN ||
           address == ADS1299_REG_LOFF_SENSP || address == ADS1299_REG_LOFF_SENSN ||
           address == ADS1299_REG_LOFF_FLIP;
}

int ads1299_sanitize_register_write(uint8_t address,
                                    uint8_t requested,
                                    ads1299_variant_t variant,
                                    uint8_t *sanitized) {
    const ads1299_register_info_t *info = ads1299_register_info(address);
    const uint8_t channel_mask = ads1299_variant_channel_mask(variant);
    if (!info || !sanitized || channel_mask == 0u || info->read_only ||
        !ads1299_register_available(address, variant)) {
        return -1;
    }

    uint8_t value = (uint8_t)(requested & info->writable_mask);
    value = (uint8_t)(value | info->required_one_mask);
    value = (uint8_t)(value & (uint8_t)~info->required_zero_mask);
    if (is_channel_mask_register(address)) value &= channel_mask;
    *sanitized = value;
    return 0;
}
