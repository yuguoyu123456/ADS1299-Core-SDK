#ifndef ADS1299_FIELD_MODEL_H
#define ADS1299_FIELD_MODEL_H

#include <stddef.h>
#include <stdint.h>

#include "ads1299_register_model.h"

#ifdef __cplusplus
extern "C" {
#endif

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
 * Encode one field into an existing register byte without touching unrelated bits.
 * The result is also checked against the register-level semantic validity model.
 */
int ads1299_field_encode(ads1299_field_id_t field,
                         uint8_t channel_1_to_8,
                         uint8_t code,
                         uint8_t current_register_value,
                         ads1299_variant_t variant,
                         uint8_t *encoded_register_value);

#ifdef __cplusplus
}
#endif

#endif /* ADS1299_FIELD_MODEL_H */
