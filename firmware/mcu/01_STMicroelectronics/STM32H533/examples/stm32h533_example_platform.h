#ifndef ADS1299_STM32H533_EXAMPLE_PLATFORM_H
#define ADS1299_STM32H533_EXAMPLE_PLATFORM_H

#include <stddef.h>
#include <stdint.h>
#include "ads1299_spi.h"

int stm32h533_ads1299_make_hal(ads1299_platform_hal_t *hal);
int stm32h533_example_write(const uint8_t *data, size_t len);
void stm32h533_example_log(const char *text);
uint32_t stm32h533_example_timestamp_us(void);

#endif /* ADS1299_STM32H533_EXAMPLE_PLATFORM_H */
