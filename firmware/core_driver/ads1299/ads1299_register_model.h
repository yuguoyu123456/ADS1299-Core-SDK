#ifndef ADS1299_REGISTER_MODEL_H
#define ADS1299_REGISTER_MODEL_H

#include <stdint.h>

#include "ads1299_regs.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Supported ADS1299-x channel-count variants. */
typedef enum {
    ADS1299_VARIANT_UNKNOWN = 0,
    ADS1299_VARIANT_4CH = 4,
    ADS1299_VARIANT_6CH = 6,
    ADS1299_VARIANT_8CH = 8
} ads1299_variant_t;

/**
 * Datasheet-derived metadata for every user-visible address 0x00..0x17.
 * writable_mask contains only semantic user-controlled bits.
 * required_one_mask/required_zero_mask encode TI's prescribed reserved-bit
 * values. ID has reset_known=0 because revision/channel fields vary by device.
 */
typedef struct {
    uint8_t address;
    uint8_t reset_value;
    uint8_t reset_known;
    uint8_t writable_mask;
    uint8_t required_one_mask;
    uint8_t required_zero_mask;
    uint8_t read_only;
} ads1299_register_info_t;

const ads1299_register_info_t *ads1299_register_info(uint8_t address);
uint8_t ads1299_variant_channel_mask(ads1299_variant_t variant);
int ads1299_register_available(uint8_t address, ads1299_variant_t variant);

/**
 * Produce the only datasheet-valid byte corresponding to a requested write.
 * Returns -1 for invalid/read-only/unavailable registers or unknown variants.
 */
int ads1299_sanitize_register_write(uint8_t address,
                                    uint8_t requested,
                                    ads1299_variant_t variant,
                                    uint8_t *sanitized);

#ifdef __cplusplus
}
#endif

#endif /* ADS1299_REGISTER_MODEL_H */
