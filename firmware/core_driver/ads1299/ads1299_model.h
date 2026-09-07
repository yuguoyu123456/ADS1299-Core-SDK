#ifndef ADS1299_MODEL_H
#define ADS1299_MODEL_H

#include <stddef.h>
#include <stdint.h>

#include "ads1299_regs.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Register safety model; normative source: TI ADS1299-x SBAS499C. */
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
 * Validate a byte exactly as it would be written to the silicon.
 * Returns 1 only when the register is writable/available, reserved bits have
 * TI-prescribed values, variant-only channel bits are clear, and no field uses
 * an encoding that TI marks Reserved or Do not use.
 */
int ads1299_register_write_value_valid(uint8_t address,
                                       uint8_t value,
                                       ads1299_variant_t variant);

/**
 * Produce a datasheet-valid byte corresponding to a requested write.
 * Reserved bits and unavailable channel bits are normalized, but semantic
 * field encodings marked Reserved/Do not use by TI are rejected rather than
 * silently changed. Returns -1 on failure.
 */
int ads1299_sanitize_register_write(uint8_t address,
                                    uint8_t requested,
                                    ads1299_variant_t variant,
                                    uint8_t *sanitized);

/* Field model; normative source: TI ADS1299-x SBAS499C. */
/** Machine-readable semantic fields from TI ADS1299-x SBAS499C. */
typedef enum {
    ADS1299_FIELD_ID_REV = 0,
    ADS1299_FIELD_ID_DEVICE,
    ADS1299_FIELD_ID_CHANNEL_CODE,
    ADS1299_FIELD_CONFIG1_DAISY_EN,
    ADS1299_FIELD_CONFIG1_CLK_EN,
    ADS1299_FIELD_CONFIG1_DR,
    ADS1299_FIELD_CONFIG2_INT_CAL,
    ADS1299_FIELD_CONFIG2_CAL_AMP,
    ADS1299_FIELD_CONFIG2_CAL_FREQ,
    ADS1299_FIELD_CONFIG3_PD_REFBUF,
    ADS1299_FIELD_CONFIG3_BIAS_MEAS,
    ADS1299_FIELD_CONFIG3_BIASREF_INT,
    ADS1299_FIELD_CONFIG3_PD_BIAS,
    ADS1299_FIELD_CONFIG3_BIAS_LOFF_SENS,
    ADS1299_FIELD_CONFIG3_BIAS_STAT,
    ADS1299_FIELD_LOFF_COMP_TH,
    ADS1299_FIELD_LOFF_CURRENT,
    ADS1299_FIELD_LOFF_FREQ,
    ADS1299_FIELD_CH_POWER_DOWN,
    ADS1299_FIELD_CH_GAIN,
    ADS1299_FIELD_CH_SRB2,
    ADS1299_FIELD_CH_MUX,
    ADS1299_FIELD_BIAS_SENSP_MASK,
    ADS1299_FIELD_BIAS_SENSN_MASK,
    ADS1299_FIELD_LOFF_SENSP_MASK,
    ADS1299_FIELD_LOFF_SENSN_MASK,
    ADS1299_FIELD_LOFF_FLIP_MASK,
    ADS1299_FIELD_LOFF_STATP_MASK,
    ADS1299_FIELD_LOFF_STATN_MASK,
    ADS1299_FIELD_GPIO_DATA,
    ADS1299_FIELD_GPIO_DIRECTION,
    ADS1299_FIELD_MISC1_SRB1,
    ADS1299_FIELD_CONFIG4_SINGLE_SHOT,
    ADS1299_FIELD_CONFIG4_LOFF_COMP_ENABLE,
    ADS1299_FIELD_COUNT
} ads1299_field_id_t;

typedef struct {
    ads1299_field_id_t id;
    const char *name;
    uint8_t register_address; /* CH fields use CH1SET as the channel-relative base. */
    uint8_t mask;
    uint8_t shift;
    uint8_t reset_code;
    uint8_t reset_known; /* ID fields are unknown because TI specifies ID reset as xxh. */
    uint8_t writable;
    uint8_t channel_relative;
    uint8_t variant_channel_masked;
    /** Bit N=1 means encoded value N is valid. Zero means all values fitting mask are valid. */
    uint16_t valid_code_bitmap;
} ads1299_field_info_t;

size_t ads1299_field_count(void);
const ads1299_field_info_t *ads1299_field_info(ads1299_field_id_t field);

/** Resolve the physical register for a field. channel is required only for CHnSET fields. */
int ads1299_field_register_address(ads1299_field_id_t field,
                                   uint8_t channel_1_to_8,
                                   ads1299_variant_t variant,
                                   uint8_t *address);

/** Decode a field code from one register byte. */
int ads1299_field_decode(ads1299_field_id_t field,
                         uint8_t register_value,
                         uint8_t *code);

/** Validate an unshifted field code, including TI do-not-use encodings and variant masks. */
int ads1299_field_code_valid(ads1299_field_id_t field,
                             uint8_t code,
                             ads1299_variant_t variant);

/**
 * Encode one field into an existing register byte without touching unrelated
 * writable fields. The returned WREG byte is normalized through the register
 * safety model, so read-only status bits are not echoed and TI-prescribed
 * reserved bits are restored automatically.
 */
int ads1299_field_encode(ads1299_field_id_t field,
                         uint8_t channel_1_to_8,
                         uint8_t code,
                         uint8_t current_register_value,
                         ads1299_variant_t variant,
                         uint8_t *encoded_register_value);

/* Physical semantics; normative source: TI ADS1299-x SBAS499C. */
/** Physical/semantic class carried by a field code. */
typedef enum {
    ADS1299_SEMANTIC_NONE = 0,
    ADS1299_SEMANTIC_BOOLEAN,
    ADS1299_SEMANTIC_ENUM,
    ADS1299_SEMANTIC_COUNT,
    ADS1299_SEMANTIC_GAIN,
    ADS1299_SEMANTIC_RATE,
    ADS1299_SEMANTIC_FREQUENCY,
    ADS1299_SEMANTIC_VOLTAGE,
    ADS1299_SEMANTIC_CURRENT,
    ADS1299_SEMANTIC_THRESHOLD_PAIR,
    ADS1299_SEMANTIC_CHANNEL_MASK
} ads1299_semantic_kind_t;

typedef enum {
    ADS1299_SEMANTIC_OK = 0,
    ADS1299_SEMANTIC_CONTEXT_REQUIRED = 1,
    ADS1299_SEMANTIC_EINVAL = -1
} ads1299_semantic_status_t;

/** Optional physical context needed to evaluate clock/reference-dependent codes. */
enum {
    ADS1299_SEMCTX_FCLK       = 1u << 0,
    ADS1299_SEMCTX_FDR        = 1u << 1,
    ADS1299_SEMCTX_VREF_SPAN  = 1u << 2,
    ADS1299_SEMCTX_AV_SUPPLY  = 1u << 3
};

typedef struct {
    uint8_t valid_mask;
    double fclk_hz;
    double fdr_hz;
    double vref_span_v; /* VREFP - VREFN */
    double avdd_v;
    double avss_v;
} ads1299_semantic_context_t;

/**
 * Machine-readable interpretation of one valid field code.
 * `name` is a stable short label; `meaning` is a concise TI-derived meaning.
 * Physical values are optional. For pair-valued thresholds, primary is the
 * positive-side percentage and secondary is the negative-side percentage.
 */
typedef struct {
    const char *name;
    const char *meaning;
    ads1299_semantic_kind_t kind;
    const char *unit;
    double primary;
    double secondary;
    uint8_t primary_valid;
    uint8_t secondary_valid;
    uint8_t required_context_mask;
} ads1299_code_descriptor_t;

/** Concise TI-derived meaning for the field itself. */
const char *ads1299_field_meaning(ads1299_field_id_t field);

/**
 * Describe a valid unshifted field code. Context is optional. The function
 * still returns names/meaning when a physical value cannot be evaluated; in
 * that case required_context_mask tells the caller which inputs are missing.
 * Returns ADS1299_SEMANTIC_OK on success and ADS1299_SEMANTIC_EINVAL for an
 * invalid field/code/variant combination.
 */
ads1299_semantic_status_t ads1299_describe_field_code(
    ads1299_field_id_t field,
    uint8_t code,
    ads1299_variant_t variant,
    const ads1299_semantic_context_t *context,
    ads1299_code_descriptor_t *descriptor);

/* Focused helpers for physical quantities explicitly defined by SBAS499C. */
ads1299_semantic_status_t ads1299_semantic_data_rate_sps(uint8_t dr_code,
                                                         double fclk_hz,
                                                         double *sps);
ads1299_semantic_status_t ads1299_semantic_test_amplitude_v(uint8_t amp_code,
                                                            double vref_span_v,
                                                            double *amplitude_v);
ads1299_semantic_status_t ads1299_semantic_test_frequency_hz(uint8_t freq_code,
                                                             double fclk_hz,
                                                             double *frequency_hz);
ads1299_semantic_status_t ads1299_semantic_biasref_internal_v(double avdd_v,
                                                              double avss_v,
                                                              double *biasref_v);
ads1299_semantic_status_t ads1299_semantic_leadoff_threshold_pct(
    uint8_t threshold_code,
    double *positive_pct,
    double *negative_pct);
ads1299_semantic_status_t ads1299_semantic_leadoff_current_a(uint8_t current_code,
                                                             double *current_a);
ads1299_semantic_status_t ads1299_semantic_leadoff_frequency_hz(
    uint8_t frequency_code,
    double fclk_hz,
    double fdr_hz,
    uint8_t available_context_mask,
    double *frequency_hz,
    uint8_t *required_context_mask);
ads1299_semantic_status_t ads1299_semantic_gain(uint8_t gain_code,
                                                double *gain_x);
#ifdef __cplusplus
}
#endif

#endif /* ADS1299_MODEL_H */
