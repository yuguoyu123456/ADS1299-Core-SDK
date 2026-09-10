#ifndef ADS1299_STM32G4_EXAMPLE_PLATFORM_H
#define ADS1299_STM32G4_EXAMPLE_PLATFORM_H

#include <stddef.h>
#include <stdint.h>
#include "ads1299_spi.h"

int stm32g4_ads1299_make_hal(ads1299_platform_hal_t *hal);
int stm32g4_example_write(const uint8_t *data, size_t len);
void stm32g4_example_log(const char *text);
uint32_t stm32g4_example_timestamp_us(void);

#endif /* ADS1299_STM32G4_EXAMPLE_PLATFORM_H */
