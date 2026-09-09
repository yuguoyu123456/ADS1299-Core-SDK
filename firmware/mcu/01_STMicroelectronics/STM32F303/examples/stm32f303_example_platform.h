#ifndef STM32F303_ADS1299_EXAMPLE_PLATFORM_H
#define STM32F303_ADS1299_EXAMPLE_PLATFORM_H

#include <stddef.h>
#include <stdint.h>
#include "ads1299_spi.h"

#ifdef __cplusplus
extern "C" {
#endif

int stm32f303_ads1299_make_hal(ads1299_platform_hal_t *hal);
int board_ads1299_hal(ads1299_platform_hal_t *hal);
void stm32f303_example_log(const char *text);
int stm32f303_example_write(const uint8_t *data, size_t len);
uint32_t stm32f303_example_timestamp_us(void);

#ifdef __cplusplus
}
#endif

#endif
