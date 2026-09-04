#ifndef ADS1299_RUNTIME_H
#define ADS1299_RUNTIME_H

#include <stdint.h>

#include "ads1299.h"
#include "ads1299_register_model.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Exit STANDBY through the WAKEUP opcode and honor TI's post-command delay. */
ads1299_status_t ads1299_wakeup(ads1299_t *dev);

/** Enter the low-power STANDBY mode. Only WAKEUP is valid until exit. */
ads1299_status_t ads1299_standby(ads1299_t *dev);

/** Drive the active-low PWDN pin. Requires port.pwdn_write. */
ads1299_status_t ads1299_set_power_down(ads1299_t *dev, int power_down);

/** Drive START high and satisfy the minimum 2-tCLK assertion interval. */
ads1299_status_t ads1299_start_pin(ads1299_t *dev);

/** Drive START low to halt pin-controlled conversions. */
ads1299_status_t ads1299_stop_pin(ads1299_t *dev);

/** Poll active-low DRDY until ready or timeout. */
ads1299_status_t ads1299_wait_drdy(ads1299_t *dev,
                                   uint32_t timeout_us,
                                   uint32_t poll_interval_us);

/**
 * Datasheet-safe single-register write.
 * Reserved bits and ADS1299-4/6/8 channel availability are enforced by the
 * register model before the WREG transaction is issued.
 */
ads1299_status_t ads1299_safe_write_register(ads1299_t *dev,
                                             uint8_t address,
                                             uint8_t requested,
                                             ads1299_variant_t variant,
                                             uint8_t *written_value);

#ifdef __cplusplus
}
#endif

#endif /* ADS1299_RUNTIME_H */
