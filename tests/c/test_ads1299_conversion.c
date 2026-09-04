#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "ads1299.h"

static int near(double a, double b, double tol) {
    const double d = (a >= b) ? (a - b) : (b - a);
    return d <= tol;
}

int main(void) {
    const double vref = 4.5;
    const double gain = 24.0;
    const double fs = vref / gain;
    const double expected_lsb = fs / 8388608.0;

    assert(ADS1299_ADC_POSITIVE_FULL_SCALE_CODE == 8388607L);
    assert(ADS1299_ADC_NEGATIVE_FULL_SCALE_CODE == -8388608L);
    assert(ADS1299_ADC_LSB_CODE_SCALE == 8388608L);
    assert(ADS1299_ADC_FULL_SCALE_CODE == ADS1299_ADC_POSITIVE_FULL_SCALE_CODE);
    assert(ADS1299_ADC_MIN_CODE == ADS1299_ADC_NEGATIVE_FULL_SCALE_CODE);

    assert(near(ads1299_lsb_volts_equation8(vref, gain), expected_lsb, 1e-18));
    assert(near(ads1299_code_to_volts_equation8(1, vref, gain), expected_lsb, 1e-18));
    assert(ads1299_code_to_volts_equation8(0, vref, gain) == 0.0);

    /* Equation 8 gives an exactly symmetric negative endpoint and a positive
     * endpoint one LSB below +FS because +2^23 is not a signed 24-bit code. */
    assert(near(ads1299_code_to_volts_equation8(
                    ADS1299_ADC_NEGATIVE_FULL_SCALE_CODE, vref, gain),
                -fs, 1e-15));
    assert(near(ads1299_code_to_volts_equation8(
                    ADS1299_ADC_POSITIVE_FULL_SCALE_CODE, vref, gain),
                fs - expected_lsb, 1e-15));

    /* Positive-full-scale normalization maps 0x7FFFFF exactly to +FS, as in
     * the project's historical conversion and common TI endpoint guidance. */
    assert(near(ads1299_code_to_volts_positive_fs(
                    ADS1299_ADC_POSITIVE_FULL_SCALE_CODE, vref, gain),
                fs, 1e-15));
    assert(near(ads1299_code_to_volts(
                    ADS1299_ADC_POSITIVE_FULL_SCALE_CODE, vref, gain),
                fs, 1e-15));
    assert(ads1299_code_to_volts_positive_fs(123456, vref, gain) ==
           ads1299_code_to_volts(123456, vref, gain));

    /* The two conventions differ by at most about one LSB at full scale and
     * are no longer silently conflated by the public API. */
    const double endpoint_delta =
        ads1299_code_to_volts_positive_fs(ADS1299_ADC_POSITIVE_FULL_SCALE_CODE,
                                          vref, gain) -
        ads1299_code_to_volts_equation8(ADS1299_ADC_POSITIVE_FULL_SCALE_CODE,
                                        vref, gain);
    assert(near(endpoint_delta, expected_lsb, 1e-15));

    assert(ads1299_lsb_volts_equation8(0.0, gain) == 0.0);
    assert(ads1299_lsb_volts_equation8(vref, 0.0) == 0.0);
    assert(ads1299_code_to_volts_equation8(1, -1.0, gain) == 0.0);

    puts("ADS1299 ADC conversion convention tests passed");
    return 0;
}
