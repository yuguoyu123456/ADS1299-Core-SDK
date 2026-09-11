#ifndef STM32H723_ADS1299_BOARD_CONFIG_H
#define STM32H723_ADS1299_BOARD_CONFIG_H

/*
 * STM32H723 ADS1299 board configuration
 *
 * Reference board: ST NUCLEO-H723ZG.
 *
 * This is the only file a new board bring-up should need to edit. Generate
 * GPIO/SPI initialization with STM32CubeMX/STM32CubeIDE, then map the generated
 * handle and GPIO symbols below. ADS1299 register behavior stays in
 * firmware/core_driver/ads1299/.
 *
 * Required SPI settings in CubeMX:
 *   master, full duplex, 8-bit, MSB first, CPOL=Low, CPHA=2 Edge (SPI Mode 1),
 *   software NSS. Keep ADS1299 CS as an ordinary output GPIO.
 */

#include "main.h" /* Cube-generated GPIO aliases plus STM32H7 HAL types. */

/* ---- SPI ---------------------------------------------------------------- */
/* Change hspi1 here if the CubeMX project uses a different SPI peripheral. */
extern SPI_HandleTypeDef hspi1;
#define ADS1299_BOARD_SPI_HANDLE (&hspi1)
#define ADS1299_BOARD_SPI_TIMEOUT_MS 100u

/* ---- ADS1299 control/data-ready GPIOs ----------------------------------- */
/*
 * Prefer naming the CubeMX GPIO labels exactly ADS1299_CS, ADS1299_DRDY,
 * ADS1299_RESET, ADS1299_PWDN and ADS1299_START. Cube then emits the
 * *_GPIO_Port and *_Pin aliases used below. If an existing project uses other
 * labels, map those aliases in this file only.
 */
#ifndef ADS1299_CS_GPIO_Port
#error "Define/map ADS1299_CS_GPIO_Port in ads1299_board_config.h"
#endif
#ifndef ADS1299_CS_Pin
#error "Define/map ADS1299_CS_Pin in ads1299_board_config.h"
#endif
#ifndef ADS1299_DRDY_GPIO_Port
#error "Define/map ADS1299_DRDY_GPIO_Port in ads1299_board_config.h"
#endif
#ifndef ADS1299_DRDY_Pin
#error "Define/map ADS1299_DRDY_Pin in ads1299_board_config.h"
#endif
#ifndef ADS1299_RESET_GPIO_Port
#error "Define/map ADS1299_RESET_GPIO_Port in ads1299_board_config.h"
#endif
#ifndef ADS1299_RESET_Pin
#error "Define/map ADS1299_RESET_Pin in ads1299_board_config.h"
#endif
#ifndef ADS1299_PWDN_GPIO_Port
#error "Define/map ADS1299_PWDN_GPIO_Port in ads1299_board_config.h"
#endif
#ifndef ADS1299_PWDN_Pin
#error "Define/map ADS1299_PWDN_Pin in ads1299_board_config.h"
#endif
#ifndef ADS1299_START_GPIO_Port
#error "Define/map ADS1299_START_GPIO_Port in ads1299_board_config.h"
#endif
#ifndef ADS1299_START_Pin
#error "Define/map ADS1299_START_Pin in ads1299_board_config.h"
#endif

#define ADS1299_BOARD_REFERENCE_NAME "NUCLEO-H723ZG"

#endif /* STM32H723_ADS1299_BOARD_CONFIG_H */
