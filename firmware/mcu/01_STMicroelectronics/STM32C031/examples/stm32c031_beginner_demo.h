#ifndef STM32C031_ADS1299_BEGINNER_DEMO_H
#define STM32C031_ADS1299_BEGINNER_DEMO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Run the complete beginner bring-up sequence after CubeMX-generated
 * initialization has completed. stream_frames == 0 selects continuous stream;
 * otherwise the function returns after the requested number of EEG packets.
 * Return 0 on success, negative stage-specific code on failure.
 */
int stm32c031_ads1299_beginner_demo(uint32_t stream_frames);

#ifdef __cplusplus
}
#endif

#endif /* STM32C031_ADS1299_BEGINNER_DEMO_H */
