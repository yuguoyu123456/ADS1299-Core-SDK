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

/** Exit STANDBY through WAKEUP and honor TI's post-command delay. */
ads1299_status_t ads1299_wakeup(ads1299_t *dev);
/** Enter STANDBY. The core rejects every SPI command except WAKEUP until exit. */
ads1299_status_t ads1299_standby(ads1299_t *dev);
/** Drive active-low PWDN. Requires port.pwdn_write. */
ads1299_status_t ads1299_set_power_down(ads1299_t *dev, int power_down);
/** START-pin control, distinct from the START opcode. */
ads1299_status_t ads1299_start_pin(ads1299_t *dev);
ads1299_status_t ads1299_stop_pin(ads1299_t *dev);
/** Poll active-low DRDY until ready or timeout. */
ads1299_status_t ads1299_wait_drdy(ads1299_t *dev,
                                   uint32_t timeout_us,
                                   uint32_t poll_interval_us);

/** Datasheet-safe single-register WREG using register metadata. */
ads1299_status_t ads1299_safe_write_register(ads1299_t *dev,
                                             uint8_t address,
                                             uint8_t requested,
                                             ads1299_variant_t variant,
                                             uint8_t *written_value);

/**
 * Datasheet-safe consecutive WREG. Every register is validated/sanitized before
 * any SPI write occurs, so an invalid/read-only/unavailable address makes the
 * whole call side-effect-free.
 */
ads1299_status_t ads1299_safe_write_registers(ads1299_t *dev,
                                              uint8_t address,
                                              const uint8_t *requested,
                                              size_t count,
                                              ads1299_variant_t variant,
                                              uint8_t *written_values);

/**
 * Safe read-modify-write. The mask itself must contain writable bits only;
 * reserved/read-only bits are rejected rather than silently modified.
 */
ads1299_status_t ads1299_safe_update_register_bits(ads1299_t *dev,
                                                   uint8_t address,
                                                   uint8_t mask,
                                                   uint8_t value,
                                                   ads1299_variant_t variant,
                                                   uint8_t *written_value);

/**
 * Field-level safe write. `code` is the unshifted datasheet field code.
 * CHnSET fields require channel_1_to_8; all other fields accept channel=0.
 * Read-only fields, unavailable channels, out-of-range codes and TI
 * Reserved/Do-not-use encodings are rejected before WREG.
 */
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
