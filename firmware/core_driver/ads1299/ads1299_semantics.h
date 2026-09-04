#ifndef ADS1299_SEMANTICS_H
#define ADS1299_SEMANTICS_H

#include <stdint.h>

#include "ads1299_field_model.h"

#ifdef __cplusplus
extern "C" {
#endif

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

#endif /* ADS1299_SEMANTICS_H */
