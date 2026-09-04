#include "ads1299.h"

/*
 * SBAS499C Rev. C has two closely related numerical conventions in its data
 * format material. Equation 8 defines one quantizer LSB as +FS / 2^23, while
 * Table 9 and TI support guidance commonly normalize the maximum positive code
 * 0x7FFFFF to +FS using (2^23 - 1). These functions keep the conventions
 * explicit instead of silently treating them as interchangeable.
 */

double ads1299_lsb_volts_equation8(double vref_volts, double gain) {
    if (gain <= 0.0 || vref_volts <= 0.0) return 0.0;
    return vref_volts /
           (gain * (double)ADS1299_ADC_LSB_CODE_SCALE);
}

double ads1299_code_to_volts_equation8(int32_t code,
                                        double vref_volts,
                                        double gain) {
    const double lsb = ads1299_lsb_volts_equation8(vref_volts, gain);
    if (lsb == 0.0) return 0.0;
    return (double)code * lsb;
}

double ads1299_code_to_volts_positive_fs(int32_t code,
                                         double vref_volts,
                                         double gain) {
    return ads1299_code_to_volts(code, vref_volts, gain);
}
