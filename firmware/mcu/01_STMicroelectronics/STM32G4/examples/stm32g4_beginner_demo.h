#ifndef ADS1299_STM32G4_BEGINNER_DEMO_H
#define ADS1299_STM32G4_BEGINNER_DEMO_H

#include <stdint.h>

/*
 * Run the standardized first-bring-up sequence.
 * stream_frames == 0 keeps streaming until reset/power-off.
 * Returns 0 on success for finite runs, negative stage-specific errors on failure.
 */
int stm32g4_ads1299_beginner_demo(uint32_t stream_frames);

#endif /* ADS1299_STM32G4_BEGINNER_DEMO_H */
