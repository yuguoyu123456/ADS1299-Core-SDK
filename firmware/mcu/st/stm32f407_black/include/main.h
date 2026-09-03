#ifndef ADS1299_STM32F407_MAIN_H
#define ADS1299_STM32F407_MAIN_H

#include "stm32f4xx_hal.h"

#define ADS_CS_GPIO_Port      GPIOB
#define ADS_CS_Pin            GPIO_PIN_6
#define ADS_DRDY_GPIO_Port    GPIOB
#define ADS_DRDY_Pin          GPIO_PIN_7
#define ADS_RESET_GPIO_Port   GPIOB
#define ADS_RESET_Pin         GPIO_PIN_8
#define ADS_START_GPIO_Port   GPIOB
#define ADS_START_Pin         GPIO_PIN_9

void Error_Handler(void);

#endif
