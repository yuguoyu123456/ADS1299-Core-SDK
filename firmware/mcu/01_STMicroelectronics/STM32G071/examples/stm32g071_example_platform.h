#ifndef STM32G071_ADS1299_EXAMPLE_PLATFORM_H
#define STM32G071_ADS1299_EXAMPLE_PLATFORM_H

#include <stddef.h>
#include <stdint.h>
#include "ads1299_spi.h"

int stm32g071_ads1299_make_hal(ads1299_platform_hal_t *hal);
int board_ads1299_hal(ads1299_platform_hal_t *hal);
int stm32g071_example_write(const uint8_t *data, size_t len);
void stm32g071_example_log(const char *text);
uint32_t stm32g071_example_timestamp_us(void);

#endif /* STM32G071_ADS1299_EXAMPLE_PLATFORM_H */
