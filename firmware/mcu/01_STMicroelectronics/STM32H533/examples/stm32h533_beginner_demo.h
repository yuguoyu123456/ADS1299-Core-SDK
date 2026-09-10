#ifndef ADS1299_STM32H533_BEGINNER_DEMO_H
#define ADS1299_STM32H533_BEGINNER_DEMO_H

#include <stdint.h>

/* stream_frames == 0 keeps streaming until reset/power-down. */
int stm32h533_ads1299_beginner_demo(uint32_t stream_frames);

#endif /* ADS1299_STM32H533_BEGINNER_DEMO_H */
