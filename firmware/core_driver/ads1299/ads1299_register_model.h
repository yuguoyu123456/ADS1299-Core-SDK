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
 * Datasheet-derived properties for one address in the 0x00..0x17 register map.
 *
 * writable_mask contains user-controlled bits. required_one_mask and
 * required_zero_mask describe reserved bits whose write value is prescribed by
 * TI. reset_known is zero only for ID because its revision/channel fields are
 * device-dependent.
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

/** Return immutable metadata for address 0x00..0x17, or NULL if invalid. */
const ads1299_register_info_t *ads1299_register_info(uint8_t address);

/** Return the physically available channel-bit mask for a 4/6/8-channel part. */
uint8_t ads1299_variant_channel_mask(ads1299_variant_t variant);

/**
 * Return non-zero when the register itself exists for the selected variant.
 * CH5/6 are unavailable on ADS1299-4; CH7/8 are available only on ADS1299.
 * All other addresses in the common map exist, although unavailable channel
 * bits in sense/lead-off registers must be written as zero.
 */
int ads1299_register_available(uint8_t address, ads1299_variant_t variant);

/**
 * Sanitize a requested register byte using TI's reserved-bit requirements and
 * variant-specific channel masks.
 *
 * Returns 0 on success and -1 for an invalid address, read-only register,
 * unsupported variant, or register unavailable on the selected variant.
 * This helper does not perform SPI I/O; it is intended for safe configuration
 * builders and validation tooling.
 */
int ads1299_sanitize_register_write(uint8_t address,
                                    uint8_t requested,
                                    ads1299_variant_t variant,
                                    uint8_t *sanitized);

#ifdef __cplusplus
}
#endif

#endif /* ADS1299_REGISTER_MODEL_H */
