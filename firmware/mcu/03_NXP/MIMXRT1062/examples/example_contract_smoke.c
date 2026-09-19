/*
 * Compile-only contract for the MIMXRT1062 beginner example surface.
 *
 * Add this file to an official MCUXpresso application together with the
 * ADS1299 RT1062 library when ADS1299_RT1062_EXAMPLE_SMOKE is enabled.
 * It deliberately does not provide main(): its purpose is to make all five
 * progressive example entry points participate in a real SDK build so API
 * drift cannot leave README-visible examples silently stale.
 *
 * Validation status: compile contract only; this is not board validation.
 */

#include <stddef.h>

/* Existing example entry points. Keep these declarations in one compile-only
 * translation unit rather than adding a second ADS1299 implementation. */
extern int ads1299_example_probe_id(void);
extern int ads1299_example_internal_test(void);
extern int ads1299_example_input_short(void);
extern int ads1299_example_eeg_250sps(void);
extern int ads1299_example_bounded_stream(void);

int ads1299_rt1062_example_contract_smoke(void)
{
    int (*const examples[])(void) = {
        ads1299_example_probe_id,
        ads1299_example_internal_test,
        ads1299_example_input_short,
        ads1299_example_eeg_250sps,
        ads1299_example_bounded_stream,
    };

    /* Referencing every symbol makes link-time validation catch renamed or
     * missing beginner flows when this object is linked by the parent SDK. */
    return (examples[0] != NULL &&
            examples[1] != NULL &&
            examples[2] != NULL &&
            examples[3] != NULL &&
            examples[4] != NULL) ? 0 : -1;
}
