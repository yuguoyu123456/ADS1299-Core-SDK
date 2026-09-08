#ifndef STM32C031_ADS1299_EXAMPLE_PLATFORM_H
#define STM32C031_ADS1299_EXAMPLE_PLATFORM_H

#include <stddef.h>
#include <stdint.h>

#include "ads1299_spi.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Repository-owned STM32Cube HAL glue for the beginner examples.
 * CubeMX/CubeIDE still owns clock setup, peripheral initialization and startup.
 * Call this only after HAL_Init(), SystemClock_Config(), MX_GPIO_Init(),
 * MX_SPI1_Init() and, when UART streaming is enabled, MX_USART1_UART_Init().
 */
int stm32c031_ads1299_make_hal(ads1299_platform_hal_t *hal);

/* Backward-compatible entry used by the pre-existing main_ads1299.c example. */
int board_ads1299_hal(ads1299_platform_hal_t *hal);

/* Beginner-visible logging/transport helpers used by the examples. */
void stm32c031_example_log(const char *text);
int stm32c031_example_write(const uint8_t *data, size_t len);
uint32_t stm32c031_example_timestamp_us(void);

#ifdef __cplusplus
}
#endif

#endif /* STM32C031_ADS1299_EXAMPLE_PLATFORM_H */
