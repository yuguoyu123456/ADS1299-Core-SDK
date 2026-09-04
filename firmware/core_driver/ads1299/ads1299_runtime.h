#ifndef ADS1299_RUNTIME_H
#define ADS1299_RUNTIME_H

#include <stddef.h>
#include <stdint.h>

#include "ads1299.h"
#include "ads1299_field_model.h"
#include "ads1299_register_model.h"

#ifdef __cplusplus
extern "C" {
#endif

ads1299_status_t ads1299_wakeup(ads1299_t *dev);
ads1299_status_t ads1299_standby(ads1299_t *dev);
ads1299_status_t ads1299_set_power_down(ads1299_t *dev, int power_down);
ads1299_status_t ads1299_start_pin(ads1299_t *dev);
ads1299_status_t ads1299_stop_pin(ads1299_t *dev);
ads1299_status_t ads1299_wait_drdy(ads1299_t *dev,
                                   uint32_t timeout_us,
                                   uint32_t poll_interval_us);

/**
 * Variant used by device-oriented high-level APIs. After ID probing this is
 * the detected ADS1299-4/-6/-8 variant. Before ID probing the historical
 * eight-channel surface is preserved by returning ADS1299_VARIANT_8CH.
 */
ads1299_variant_t ads1299_effective_variant(const ads1299_t *dev);

/**
 * Read and decode one machine-readable field. Forbidden/reserved field states
 * are reported as ADS1299_EVERIFY instead of being silently accepted.
 */
ads1299_status_t ads1299_read_field(ads1299_t *dev,
                                    ads1299_field_id_t field,
                                    uint8_t channel_1_to_8,
                                    ads1299_variant_t variant,
                                    uint8_t *code);

/**
 * Normalizing safe write. The requested byte is sanitized through the TI
 * register model before WREG: prescribed reserved bits and variant masks are
 * normalized, while read-only/unavailable registers and forbidden semantic
 * encodings are rejected. `written_value` returns the actual WREG byte.
 */
ads1299_status_t ads1299_safe_write_register(ads1299_t *dev,
                                             uint8_t address,
                                             uint8_t requested,
                                             ads1299_variant_t variant,
                                             uint8_t *written_value);
ads1299_status_t ads1299_safe_write_registers(ads1299_t *dev,
                                              uint8_t address,
                                              const uint8_t *requested,
                                              size_t count,
                                              ads1299_variant_t variant,
                                              uint8_t *written_values);

/**
 * Strict write: the caller-provided byte must already be exactly datasheet
 * valid. Nothing is normalized. Any wrong reserved bit, unavailable variant
 * bit, read-only register, or TI do-not-use encoding rejects the complete call
 * before SPI I/O. Use this when configuration errors must never be hidden.
 */
ads1299_status_t ads1299_strict_write_register(ads1299_t *dev,
                                               uint8_t address,
                                               uint8_t value,
                                               ads1299_variant_t variant);
ads1299_status_t ads1299_strict_write_registers(ads1299_t *dev,
                                                uint8_t address,
                                                const uint8_t *values,
                                                size_t count,
                                                ads1299_variant_t variant);

ads1299_status_t ads1299_safe_update_register_bits(ads1299_t *dev,
                                                   uint8_t address,
                                                   uint8_t mask,
                                                   uint8_t value,
                                                   ads1299_variant_t variant,
                                                   uint8_t *written_value);

ads1299_status_t ads1299_safe_write_field(ads1299_t *dev,
                                          ads1299_field_id_t field,
                                          uint8_t channel_1_to_8,
                                          uint8_t code,
                                          ads1299_variant_t variant,
                                          uint8_t *written_register_value);

#ifdef __cplusplus
}
#endif

#endif /* ADS1299_RUNTIME_H */
