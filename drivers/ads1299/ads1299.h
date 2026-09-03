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
 * @brief Portable, controller-independent ADS1299 device API.
 *
 * The driver intentionally separates the ADS1299 programming model from MCU
 * details. A platform port supplies SPI, CS, RESET, START, DRDY and delay
 * callbacks; this layer owns command framing, register access, configuration,
 * frame decoding and engineering diagnostics.
 *
 * Design rules:
 * - ADS1299 SPI uses Mode 1 (CPOL=0, CPHA=1), MSB first.
 * - Register access is not performed while RDATAC remains active. Existing
 *   low-level read/write helpers stop RDATAC first and leave the device in
 *   command mode; application code must explicitly restart streaming.
 * - Reserved register bits are preserved or written with datasheet-required
 *   values by high-level helpers.
 * - Values in this API are ADC codes unless a function name explicitly says
 *   volts or microvolts.
 *
 * Primary device specification: Texas Instruments ADS1299-x datasheet,
 * SBAS499C. This library is for research/engineering use and is not a medical
 * device software stack.
 */

typedef enum {
    ADS1299_OK = 0,
    ADS1299_EINVAL = -1,
    ADS1299_EIO = -2,
    ADS1299_ETIMEOUT = -3,
    ADS1299_ESTATE = -4,
    ADS1299_EVERIFY = -5
} ads1299_status_t;

/** One decoded ADS1299 conversion frame: 24-bit status + 8 x signed 24-bit. */
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
    uint8_t channel_count; /* 4, 6, 8, or 0 if the code is unexpected */
    uint8_t is_ads1299_family;
} ads1299_device_id_t;

/** Snapshot of every user-visible register from ID (0x00) through CONFIG4. */
typedef struct {
    uint8_t value[ADS1299_REGISTER_COUNT];
} ads1299_register_dump_t;

/**
 * BIAS/reference configuration.
 *
 * positive_sense_mask and negative_sense_mask are channel masks: bit0=CH1,
 * bit7=CH8. Select only channels that are actually connected and suitable for
 * common-mode sensing in the user's montage.
 */
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
    uint8_t comparator_threshold_code; /* ADS1299_LOFF_COMP_* */
    uint8_t current_code;              /* ADS1299_LOFF_CURRENT_* */
    uint8_t frequency_code;            /* ADS1299_LOFF_FREQ_* */
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
 * Portable ADS1299 handle.
 *
 * Keep this object alive for the lifetime of the platform context stored in
 * port.user. The driver does not allocate memory dynamically.
 */
typedef struct {
    ads1299_port_t port;
    uint8_t continuous_mode;
} ads1299_t;

/* -------------------------------------------------------------------------- */
/* Lifecycle and raw commands                                                  */
/* -------------------------------------------------------------------------- */

/** Initialize the portable handle and place CS/START/RESET pins in idle state. */
ads1299_status_t ads1299_init(ads1299_t *dev, const ads1299_port_t *port);

/** Send one ADS1299 command opcode. Prefer named helpers for common commands. */
ads1299_status_t ads1299_command(ads1299_t *dev, uint8_t command);

/** Toggle the external RESET pin using the timing callback in the platform port. */
ads1299_status_t ads1299_hardware_reset(ads1299_t *dev);

/** Send the RESET SPI command. */
ads1299_status_t ads1299_reset_command(ads1299_t *dev);

/** START conversions through the SPI command interface. */
ads1299_status_t ads1299_start(ads1299_t *dev);

/** STOP conversions through the SPI command interface. */
ads1299_status_t ads1299_stop(ads1299_t *dev);

/** Enter read-data-continuous mode. */
ads1299_status_t ads1299_rdatac(ads1299_t *dev);

/** Leave read-data-continuous mode so registers can be accessed. */
ads1299_status_t ads1299_sdatac(ads1299_t *dev);

/* -------------------------------------------------------------------------- */
/* Register access and diagnostics                                             */
/* -------------------------------------------------------------------------- */

ads1299_status_t ads1299_read_register(ads1299_t *dev,
                                       uint8_t address,
                                       uint8_t *value);
ads1299_status_t ads1299_write_register(ads1299_t *dev,
                                        uint8_t address,
                                        uint8_t value);
ads1299_status_t ads1299_read_registers(ads1299_t *dev,
                                        uint8_t address,
                                        uint8_t *values,
                                        size_t count);
ads1299_status_t ads1299_write_registers(ads1299_t *dev,
                                         uint8_t address,
                                         const uint8_t *values,
                                         size_t count);

/** Read-modify-write selected bits while preserving unrelated register fields. */
ads1299_status_t ads1299_update_register_bits(ads1299_t *dev,
                                              uint8_t address,
                                              uint8_t mask,
                                              uint8_t value);

/** Read back a register and compare it to an expected value through a mask. */
ads1299_status_t ads1299_verify_register(ads1299_t *dev,
                                         uint8_t address,
                                         uint8_t expected,
                                         uint8_t mask,
                                         uint8_t *actual);

/** Read and decode the ADS1299-x ID register. */
ads1299_status_t ads1299_read_device_id(ads1299_t *dev,
                                        ads1299_device_id_t *id);

/** Read registers 0x00..0x17 into one deterministic diagnostic snapshot. */
ads1299_status_t ads1299_read_register_dump(ads1299_t *dev,
                                            ads1299_register_dump_t *dump);

/* -------------------------------------------------------------------------- */
/* Global clock / sampling / topology                                          */
/* -------------------------------------------------------------------------- */

/** Set CONFIG1 DR[2:0]. ADS1299_DR_RESERVED is rejected. */
ads1299_status_t ads1299_set_data_rate(ads1299_t *dev, uint8_t dr_code);

/** Enable/disable the CLK pin output while preserving data-rate/daisy fields. */
ads1299_status_t ads1299_set_clock_output(ads1299_t *dev, int enable);

/**
 * Select daisy-chain mode (enable=1) or multiple-readback mode (enable=0).
 * TI's DAISY_EN bit is active-low for daisy behavior; the function hides that
 * naming trap from application code.
 */
ads1299_status_t ads1299_set_daisy_chain_mode(ads1299_t *dev, int enable);

/** Select continuous conversion or CONFIG4 single-shot conversion mode. */
ads1299_status_t ads1299_set_single_shot_mode(ads1299_t *dev, int enable);

/* -------------------------------------------------------------------------- */
/* Channel configuration                                                       */
/* -------------------------------------------------------------------------- */

/**
 * Configure one channel (1..8).
 * gain_code must be ADS1299_GAIN_* and mux_code ADS1299_MUX_*.
 */
ads1299_status_t ads1299_set_channel(ads1299_t *dev,
                                     uint8_t channel_1_to_8,
                                     uint8_t gain_code,
                                     uint8_t mux_code,
                                     int srb2,
                                     int power_down);

/** Power down a channel and route it to the internal shorted-input mux. */
ads1299_status_t ads1299_power_down_channel(ads1299_t *dev,
                                            uint8_t channel_1_to_8);

/** Route SRB1 to all inverting channel inputs through MISC1 bit5. */
ads1299_status_t ads1299_set_srb1(ads1299_t *dev, int enable);

/** Set/clear SRB2 for one channel without changing gain/mux/power fields. */
ads1299_status_t ads1299_set_channel_srb2(ads1299_t *dev,
                                          uint8_t channel_1_to_8,
                                          int enable);

/* -------------------------------------------------------------------------- */
/* Reference / BIAS drive                                                      */
/* -------------------------------------------------------------------------- */

/** Apply reference-buffer, BIAS amplifier and BIAS_SENSP/N configuration. */
ads1299_status_t ads1299_configure_bias(ads1299_t *dev,
                                        const ads1299_bias_config_t *config);

/** Read only the BIAS lead-off status bit from CONFIG3. */
ads1299_status_t ads1299_read_bias_status(ads1299_t *dev,
                                          uint8_t *bias_off);

/* -------------------------------------------------------------------------- */
/* Lead-off / electrode contact                                                */
/* -------------------------------------------------------------------------- */

/** Configure LOFF, LOFF_SENSP/N, LOFF_FLIP and CONFIG4 comparator enable. */
ads1299_status_t ads1299_configure_lead_off(ads1299_t *dev,
                                            const ads1299_leadoff_config_t *config);

/** Read LOFF_STATP/N and BIAS_STAT. */
ads1299_status_t ads1299_read_lead_off_status(ads1299_t *dev,
                                              ads1299_leadoff_status_t *status);

/* -------------------------------------------------------------------------- */
/* Internal self-test / noise-test profiles                                    */
/* -------------------------------------------------------------------------- */

/** Construct a CONFIG2 byte that preserves TI-required reserved bits. */
uint8_t ads1299_make_internal_test_config2(int amplitude_x2,
                                           uint8_t freq_code);

/** Route all eight channels to the internally generated test source. */
ads1299_status_t ads1299_configure_internal_test(ads1299_t *dev,
                                                 uint8_t gain_code,
                                                 int amplitude_x2,
                                                 uint8_t freq_code);

/** Route all eight channel inputs to the internal short for noise evaluation. */
ads1299_status_t ads1299_configure_input_short_test(ads1299_t *dev,
                                                    uint8_t gain_code);

/* -------------------------------------------------------------------------- */
/* ADS1299 GPIO                                                                */
/* -------------------------------------------------------------------------- */

/**
 * Configure ADS1299 GPIO1..GPIO4.
 * direction_mask bit=1 means input; bit=0 means output.
 * output_value_mask is written to GPIOD[4:1].
 */
ads1299_status_t ads1299_gpio_configure(ads1299_t *dev,
                                         uint8_t direction_mask,
                                         uint8_t output_value_mask);

/** Update GPIO output data while preserving direction bits. */
ads1299_status_t ads1299_gpio_write(ads1299_t *dev,
                                    uint8_t output_value_mask);

/** Read current external pin state from GPIOD[4:1]. */
ads1299_status_t ads1299_gpio_read(ads1299_t *dev,
                                   uint8_t *pin_state_mask);

/* -------------------------------------------------------------------------- */
/* Data acquisition                                                            */
/* -------------------------------------------------------------------------- */

/** Clock exactly one 27-byte frame while RDATAC mode is active. */
ads1299_status_t ads1299_read_frame_continuous(ads1299_t *dev,
                                               ads1299_frame_t *frame);

/** Issue RDATA and then clock one 27-byte frame while RDATAC is inactive. */
ads1299_status_t ads1299_read_frame_rdata(ads1299_t *dev,
                                          ads1299_frame_t *frame);

/* -------------------------------------------------------------------------- */
/* Numeric conversion helpers                                                  */
/* -------------------------------------------------------------------------- */

/** Sign-extend a 24-bit two's-complement ADS1299 sample into int32_t. */
int32_t ads1299_sign_extend24(uint32_t value24);

/** Convert a raw ADC code to volts using the configured VREF and PGA gain. */
double ads1299_code_to_volts(int32_t code,
                             double vref_volts,
                             double gain);

/** Convert a raw ADC code directly to microvolts. */
double ads1299_code_to_microvolts(int32_t code,
                                  double vref_volts,
                                  double gain);

#ifdef __cplusplus
}
#endif

#endif /* ADS1299_H */
