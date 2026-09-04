#ifndef ADS1299_H
#define ADS1299_H

#include <stddef.h>
#include <stdint.h>

#include "ads1299_port.h"
#include "ads1299_regs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file ads1299.h
 * @brief Portable, controller-independent ADS1299-x device API.
 *
 * Primary specification: Texas Instruments ADS1299-x datasheet SBAS499C.
 * This layer owns command framing, register access, configuration, frame
 * decoding and engineering diagnostics; platform ports only provide SPI/GPIO
 * and delays.
 */

typedef enum {
    ADS1299_OK = 0,
    ADS1299_EINVAL = -1,
    ADS1299_EIO = -2,
    ADS1299_ETIMEOUT = -3,
    ADS1299_ESTATE = -4,
    ADS1299_EVERIFY = -5
} ads1299_status_t;

/** One decoded ADS1299 conversion frame: 24-bit status + up to 8 channels. */
typedef struct {
    uint8_t status[ADS1299_STATUS_BYTES];
    int32_t channel[ADS1299_CHANNEL_COUNT];
} ads1299_frame_t;

/** Device identity decoded from the read-only ID register. */
typedef struct {
    uint8_t raw;
    uint8_t revision;
    uint8_t device_code;
    uint8_t channel_code;
    uint8_t channel_count; /* 4, 6, 8, or 0 if unexpected */
    uint8_t is_ads1299_family;
} ads1299_device_id_t;

/** Snapshot of every user-visible register from ID (0x00) through CONFIG4. */
typedef struct {
    uint8_t value[ADS1299_REGISTER_COUNT];
} ads1299_register_dump_t;

/** Complete CONFIG2 test-signal generator configuration. */
typedef struct {
    uint8_t use_internal_source; /* 0=external test source, 1=internal generator */
    uint8_t amplitude_x2;        /* CAL_AMP: 0=1x, 1=2x */
    uint8_t frequency_code;      /* ADS1299_TEST_FREQ_*; reserved code rejected */
} ads1299_test_signal_config_t;

/** BIAS/reference configuration covering all writable CONFIG3 BIAS fields. */
typedef struct {
    uint8_t enable_reference_buffer;
    uint8_t use_internal_bias_reference;
    uint8_t enable_bias_buffer;
    uint8_t enable_bias_measurement;
    uint8_t enable_bias_leadoff_sense;
    uint8_t positive_sense_mask;
    uint8_t negative_sense_mask;
} ads1299_bias_config_t;

/** Electrode lead-off generator/comparator configuration. */
typedef struct {
    uint8_t comparator_threshold_code;
    uint8_t current_code;
    uint8_t frequency_code;
    uint8_t positive_sense_mask;
    uint8_t negative_sense_mask;
    uint8_t current_flip_mask;
    uint8_t enable_comparators;
} ads1299_leadoff_config_t;

/** Latest lead-off status read directly from the status registers. */
typedef struct {
    uint8_t positive_off_mask;
    uint8_t negative_off_mask;
    uint8_t bias_off;
} ads1299_leadoff_status_t;

/**
 * Portable ADS1299-x handle.
 * channel_count becomes 4/6/8 after ads1299_read_device_id() succeeds on an
 * ADS1299-family device; zero means not yet identified. standby_mode mirrors
 * entry/exit through the driver's named STANDBY/WAKEUP helpers.
 */
typedef struct {
    ads1299_port_t port;
    uint8_t continuous_mode;
    uint8_t standby_mode;
    uint8_t channel_count;
} ads1299_t;

/* Lifecycle and commands */
ads1299_status_t ads1299_init(ads1299_t *dev, const ads1299_port_t *port);
ads1299_status_t ads1299_command(ads1299_t *dev, uint8_t command);
ads1299_status_t ads1299_hardware_reset(ads1299_t *dev);
ads1299_status_t ads1299_reset_command(ads1299_t *dev);
ads1299_status_t ads1299_start(ads1299_t *dev);
ads1299_status_t ads1299_stop(ads1299_t *dev);
ads1299_status_t ads1299_rdatac(ads1299_t *dev);
ads1299_status_t ads1299_sdatac(ads1299_t *dev);

/* Register access and diagnostics */
ads1299_status_t ads1299_read_register(ads1299_t *dev, uint8_t address, uint8_t *value);
ads1299_status_t ads1299_write_register(ads1299_t *dev, uint8_t address, uint8_t value);
ads1299_status_t ads1299_read_registers(ads1299_t *dev, uint8_t address,
                                        uint8_t *values, size_t count);
ads1299_status_t ads1299_write_registers(ads1299_t *dev, uint8_t address,
                                         const uint8_t *values, size_t count);
ads1299_status_t ads1299_update_register_bits(ads1299_t *dev, uint8_t address,
                                              uint8_t mask, uint8_t value);
ads1299_status_t ads1299_verify_register(ads1299_t *dev, uint8_t address,
                                         uint8_t expected, uint8_t mask,
                                         uint8_t *actual);
/** Read ID and cache the physical 4/6/8-channel count in dev on success. */
ads1299_status_t ads1299_read_device_id(ads1299_t *dev, ads1299_device_id_t *id);
ads1299_status_t ads1299_read_register_dump(ads1299_t *dev,
                                            ads1299_register_dump_t *dump);

/* Global clock / sampling / topology */
ads1299_status_t ads1299_set_data_rate(ads1299_t *dev, uint8_t dr_code);
ads1299_status_t ads1299_set_clock_output(ads1299_t *dev, int enable);
ads1299_status_t ads1299_set_daisy_chain_mode(ads1299_t *dev, int enable);
ads1299_status_t ads1299_set_single_shot_mode(ads1299_t *dev, int enable);

/* Channel configuration */
ads1299_status_t ads1299_set_channel(ads1299_t *dev, uint8_t channel_1_to_8,
                                     uint8_t gain_code, uint8_t mux_code,
                                     int srb2, int power_down);
ads1299_status_t ads1299_power_down_channel(ads1299_t *dev,
                                            uint8_t channel_1_to_8);
ads1299_status_t ads1299_set_srb1(ads1299_t *dev, int enable);
ads1299_status_t ads1299_set_channel_srb2(ads1299_t *dev,
                                          uint8_t channel_1_to_8, int enable);

/* Reference / BIAS */
ads1299_status_t ads1299_configure_bias(ads1299_t *dev,
                                        const ads1299_bias_config_t *config);
ads1299_status_t ads1299_read_bias_status(ads1299_t *dev, uint8_t *bias_off);

/* Lead-off */
ads1299_status_t ads1299_configure_lead_off(ads1299_t *dev,
                                            const ads1299_leadoff_config_t *config);
ads1299_status_t ads1299_read_lead_off_status(ads1299_t *dev,
                                              ads1299_leadoff_status_t *status);

/* CONFIG2 test source and diagnostic profiles */
/** Build a datasheet-valid CONFIG2 byte, including external/internal source. */
uint8_t ads1299_make_test_config2(const ads1299_test_signal_config_t *config);
/** Program all meaningful CONFIG2 test-generator fields. */
ads1299_status_t ads1299_configure_test_signal(
    ads1299_t *dev, const ads1299_test_signal_config_t *config);
/** Backward-compatible convenience builder for the internal generator. */
uint8_t ads1299_make_internal_test_config2(int amplitude_x2, uint8_t freq_code);
ads1299_status_t ads1299_configure_internal_test(ads1299_t *dev,
                                                 uint8_t gain_code,
                                                 int amplitude_x2,
                                                 uint8_t freq_code);
ads1299_status_t ads1299_configure_input_short_test(ads1299_t *dev,
                                                    uint8_t gain_code);

/* ADS1299 GPIO */
ads1299_status_t ads1299_gpio_configure(ads1299_t *dev, uint8_t direction_mask,
                                        uint8_t output_value_mask);
ads1299_status_t ads1299_gpio_write(ads1299_t *dev, uint8_t output_value_mask);
ads1299_status_t ads1299_gpio_read(ads1299_t *dev, uint8_t *pin_state_mask);

/* Legacy fixed-8-channel acquisition helpers. Variant-aware frame helpers are
 * provided by ads1299_frame.h for ADS1299-4/-6/8. */
ads1299_status_t ads1299_read_frame_continuous(ads1299_t *dev,
                                               ads1299_frame_t *frame);
ads1299_status_t ads1299_read_frame_rdata(ads1299_t *dev,
                                          ads1299_frame_t *frame);

/* Numeric conversion */
int32_t ads1299_sign_extend24(uint32_t value24);
double ads1299_code_to_volts(int32_t code, double vref_volts, double gain);
double ads1299_code_to_microvolts(int32_t code, double vref_volts, double gain);

#ifdef __cplusplus
}
#endif

#endif /* ADS1299_H */
