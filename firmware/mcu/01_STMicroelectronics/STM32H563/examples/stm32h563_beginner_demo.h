#ifndef ADS1299_STM32H563_BEGINNER_DEMO_H
#define ADS1299_STM32H563_BEGINNER_DEMO_H

#include <stdint.h>

/* stream_frames == 0 keeps streaming until reset/power-down. */
int stm32h563_ads1299_beginner_demo(uint32_t stream_frames);

#endif /* ADS1299_STM32H563_BEGINNER_DEMO_H */
