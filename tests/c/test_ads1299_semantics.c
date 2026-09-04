#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "ads1299_semantics.h"

static int near(double a, double b, double tol) {
    const double d = (a >= b) ? (a - b) : (b - a);
    return d <= tol;
}

static ads1299_semantic_context_t full_context(void) {
    ads1299_semantic_context_t ctx = {0};
    ctx.valid_mask = ADS1299_SEMCTX_FCLK | ADS1299_SEMCTX_FDR |
                     ADS1299_SEMCTX_VREF_SPAN | ADS1299_SEMCTX_AV_SUPPLY;
    ctx.fclk_hz = 2048000.0;
    ctx.fdr_hz = 250.0;
    ctx.vref_span_v = 4.5;
    ctx.avdd_v = 5.0;
    ctx.avss_v = 0.0;
    return ctx;
}

static void test_ti_physical_helpers(void) {
    double value = 0.0;
    assert(ads1299_semantic_data_rate_sps(0u, 2048000.0, &value) ==
           ADS1299_SEMANTIC_OK);
    assert(near(value, 16000.0, 1e-9));
    assert(ads1299_semantic_data_rate_sps(6u, 2048000.0, &value) ==
           ADS1299_SEMANTIC_OK);
    assert(near(value, 250.0, 1e-9));
    assert(ads1299_semantic_data_rate_sps(0u, 4096000.0, &value) ==
           ADS1299_SEMANTIC_OK);
    assert(near(value, 32000.0, 1e-9));
    assert(ads1299_semantic_data_rate_sps(7u, 2048000.0, &value) ==
           ADS1299_SEMANTIC_EINVAL);

    assert(ads1299_semantic_test_amplitude_v(0u, 4.5, &value) ==
           ADS1299_SEMANTIC_OK);
    assert(near(value, 0.001875, 1e-12));
    assert(ads1299_semantic_test_amplitude_v(1u, 4.5, &value) ==
           ADS1299_SEMANTIC_OK);
    assert(near(value, 0.00375, 1e-12));

    assert(ads1299_semantic_test_frequency_hz(0u, 2048000.0, &value) ==
           ADS1299_SEMANTIC_OK);
    assert(near(value, 0.9765625, 1e-12));
    assert(ads1299_semantic_test_frequency_hz(1u, 2048000.0, &value) ==
           ADS1299_SEMANTIC_OK);
    assert(near(value, 1.953125, 1e-12));
    assert(ads1299_semantic_test_frequency_hz(3u, 0.0, &value) ==
           ADS1299_SEMANTIC_OK);
    assert(value == 0.0);
    assert(ads1299_semantic_test_frequency_hz(2u, 2048000.0, &value) ==
           ADS1299_SEMANTIC_EINVAL);

    assert(ads1299_semantic_biasref_internal_v(5.0, 0.0, &value) ==
           ADS1299_SEMANTIC_OK);
    assert(near(value, 2.5, 1e-12));
    assert(ads1299_semantic_biasref_internal_v(2.5, -2.5, &value) ==
           ADS1299_SEMANTIC_OK);
    assert(near(value, 0.0, 1e-12));

    double positive = 0.0, negative = 0.0;
    assert(ads1299_semantic_leadoff_threshold_pct(0u, &positive, &negative) ==
           ADS1299_SEMANTIC_OK);
    assert(near(positive, 95.0, 1e-12) && near(negative, 5.0, 1e-12));
    assert(ads1299_semantic_leadoff_threshold_pct(7u, &positive, &negative) ==
           ADS1299_SEMANTIC_OK);
    assert(near(positive, 70.0, 1e-12) && near(negative, 30.0, 1e-12));

    assert(ads1299_semantic_leadoff_current_a(0u, &value) == ADS1299_SEMANTIC_OK);
    assert(near(value, 6e-9, 1e-15));
    assert(ads1299_semantic_leadoff_current_a(1u, &value) == ADS1299_SEMANTIC_OK);
    assert(near(value, 24e-9, 1e-15));
    assert(ads1299_semantic_leadoff_current_a(2u, &value) == ADS1299_SEMANTIC_OK);
    assert(near(value, 6e-6, 1e-12));
    assert(ads1299_semantic_leadoff_current_a(3u, &value) == ADS1299_SEMANTIC_OK);
    assert(near(value, 24e-6, 1e-12));

    uint8_t required = 0u;
    assert(ads1299_semantic_leadoff_frequency_hz(
               1u, 2048000.0, 0.0, ADS1299_SEMCTX_FCLK, &value, &required) ==
           ADS1299_SEMANTIC_OK);
    assert(near(value, 7.8125, 1e-12));
    assert(ads1299_semantic_leadoff_frequency_hz(
               2u, 2048000.0, 0.0, ADS1299_SEMCTX_FCLK, &value, &required) ==
           ADS1299_SEMANTIC_OK);
    assert(near(value, 31.25, 1e-12));
    assert(ads1299_semantic_leadoff_frequency_hz(
               3u, 0.0, 250.0, ADS1299_SEMCTX_FDR, &value, &required) ==
           ADS1299_SEMANTIC_OK);
    assert(near(value, 62.5, 1e-12));
    assert(ads1299_semantic_leadoff_frequency_hz(
               3u, 0.0, 0.0, 0u, &value, &required) ==
           ADS1299_SEMANTIC_CONTEXT_REQUIRED);
    assert(required == ADS1299_SEMCTX_FDR);

    assert(ads1299_semantic_gain(6u, &value) == ADS1299_SEMANTIC_OK);
    assert(near(value, 24.0, 1e-12));
    assert(ads1299_semantic_gain(7u, &value) == ADS1299_SEMANTIC_EINVAL);
}

static void test_context_aware_descriptors(void) {
    ads1299_code_descriptor_t d = {0};
    ads1299_semantic_context_t ctx = full_context();

    assert(ads1299_describe_field_code(ADS1299_FIELD_CONFIG1_DR, 6u,
                                       ADS1299_VARIANT_8CH, &ctx, &d) ==
           ADS1299_SEMANTIC_OK);
    assert(d.name != NULL && d.meaning != NULL && d.kind == ADS1299_SEMANTIC_RATE);
    assert(d.primary_valid && near(d.primary, 250.0, 1e-12));
    assert(d.unit != NULL);

    assert(ads1299_describe_field_code(ADS1299_FIELD_CONFIG2_CAL_AMP, 0u,
                                       ADS1299_VARIANT_8CH, &ctx, &d) ==
           ADS1299_SEMANTIC_OK);
    assert(d.primary_valid && near(d.primary, 0.001875, 1e-12));

    assert(ads1299_describe_field_code(ADS1299_FIELD_CONFIG3_BIASREF_INT, 1u,
                                       ADS1299_VARIANT_8CH, &ctx, &d) ==
           ADS1299_SEMANTIC_OK);
    assert(d.primary_valid && near(d.primary, 2.5, 1e-12));

    assert(ads1299_describe_field_code(ADS1299_FIELD_LOFF_COMP_TH, 3u,
                                       ADS1299_VARIANT_8CH, &ctx, &d) ==
           ADS1299_SEMANTIC_OK);
    assert(d.primary_valid && d.secondary_valid);
    assert(near(d.primary, 87.5, 1e-12) && near(d.secondary, 12.5, 1e-12));

    assert(ads1299_describe_field_code(ADS1299_FIELD_LOFF_FREQ, 3u,
                                       ADS1299_VARIANT_8CH, &ctx, &d) ==
           ADS1299_SEMANTIC_OK);
    assert(d.primary_valid && near(d.primary, 62.5, 1e-12));

    assert(ads1299_describe_field_code(ADS1299_FIELD_CH_GAIN, 6u,
                                       ADS1299_VARIANT_8CH, &ctx, &d) ==
           ADS1299_SEMANTIC_OK);
    assert(d.primary_valid && near(d.primary, 24.0, 1e-12));

    assert(ads1299_describe_field_code(ADS1299_FIELD_CH_MUX, ADS1299_MUX_TEMP,
                                       ADS1299_VARIANT_8CH, &ctx, &d) ==
           ADS1299_SEMANTIC_OK);
    assert(d.name != NULL && d.kind == ADS1299_SEMANTIC_ENUM);
}

static void test_missing_context_is_explicit(void) {
    ads1299_code_descriptor_t d = {0};

    assert(ads1299_describe_field_code(ADS1299_FIELD_CONFIG1_DR, 6u,
                                       ADS1299_VARIANT_8CH, NULL, &d) ==
           ADS1299_SEMANTIC_OK);
    assert(!d.primary_valid && d.required_context_mask == ADS1299_SEMCTX_FCLK);

    assert(ads1299_describe_field_code(ADS1299_FIELD_CONFIG2_CAL_AMP, 1u,
                                       ADS1299_VARIANT_8CH, NULL, &d) ==
           ADS1299_SEMANTIC_OK);
    assert(!d.primary_valid && d.required_context_mask == ADS1299_SEMCTX_VREF_SPAN);

    assert(ads1299_describe_field_code(ADS1299_FIELD_CONFIG2_CAL_FREQ, 3u,
                                       ADS1299_VARIANT_8CH, NULL, &d) ==
           ADS1299_SEMANTIC_OK);
    assert(d.primary_valid && d.primary == 0.0 && d.required_context_mask == 0u);

    assert(ads1299_describe_field_code(ADS1299_FIELD_CONFIG3_BIASREF_INT, 1u,
                                       ADS1299_VARIANT_8CH, NULL, &d) ==
           ADS1299_SEMANTIC_OK);
    assert(!d.primary_valid && d.required_context_mask == ADS1299_SEMCTX_AV_SUPPLY);

    assert(ads1299_describe_field_code(ADS1299_FIELD_LOFF_FREQ, 3u,
                                       ADS1299_VARIANT_8CH, NULL, &d) ==
           ADS1299_SEMANTIC_OK);
    assert(!d.primary_valid && d.required_context_mask == ADS1299_SEMCTX_FDR);
}

static void test_invalid_codes_and_variants_rejected(void) {
    ads1299_code_descriptor_t d = {0};
    ads1299_semantic_context_t ctx = full_context();
    assert(ads1299_describe_field_code(ADS1299_FIELD_CONFIG1_DR, 7u,
                                       ADS1299_VARIANT_8CH, &ctx, &d) ==
           ADS1299_SEMANTIC_EINVAL);
    assert(ads1299_describe_field_code(ADS1299_FIELD_CONFIG2_CAL_FREQ, 2u,
                                       ADS1299_VARIANT_8CH, &ctx, &d) ==
           ADS1299_SEMANTIC_EINVAL);
    assert(ads1299_describe_field_code(ADS1299_FIELD_CH_GAIN, 7u,
                                       ADS1299_VARIANT_8CH, &ctx, &d) ==
           ADS1299_SEMANTIC_EINVAL);
    assert(ads1299_describe_field_code(ADS1299_FIELD_BIAS_SENSP_MASK, 0x10u,
                                       ADS1299_VARIANT_4CH, &ctx, &d) ==
           ADS1299_SEMANTIC_EINVAL);
}

static void test_every_valid_field_code_is_describable(void) {
    const ads1299_variant_t variants[3] = {
        ADS1299_VARIANT_4CH, ADS1299_VARIANT_6CH, ADS1299_VARIANT_8CH
    };
    ads1299_semantic_context_t ctx = full_context();
    ads1299_code_descriptor_t d = {0};

    for (int f = 0; f < ADS1299_FIELD_COUNT; ++f) {
        const ads1299_field_info_t *info = ads1299_field_info((ads1299_field_id_t)f);
        assert(info != NULL);
        const uint8_t max_code = (uint8_t)(info->mask >> info->shift);
        for (size_t v = 0; v < 3u; ++v) {
            for (unsigned code = 0u; code <= max_code; ++code) {
                const int valid = ads1299_field_code_valid((ads1299_field_id_t)f,
                                                           (uint8_t)code,
                                                           variants[v]);
                const ads1299_semantic_status_t rc = ads1299_describe_field_code(
                    (ads1299_field_id_t)f, (uint8_t)code, variants[v], &ctx, &d);
                if (valid) {
                    assert(rc == ADS1299_SEMANTIC_OK);
                    assert(d.name != NULL && d.meaning != NULL);
                } else {
                    assert(rc == ADS1299_SEMANTIC_EINVAL);
                }
            }
        }
    }
}

int main(void) {
    test_ti_physical_helpers();
    test_context_aware_descriptors();
    test_missing_context_is_explicit();
    test_invalid_codes_and_variants_rejected();
    test_every_valid_field_code_is_describable();
    puts("ADS1299 semantic-value tests passed");
    return 0;
}
