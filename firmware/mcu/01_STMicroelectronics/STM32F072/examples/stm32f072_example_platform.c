#include "stm32f072_example_platform.h"

#include <string.h>
#include "main.h"
#include "stm32f0xx_hal.h"
#include "../board/board_config.h"

extern SPI_HandleTypeDef ADS1299_BOARD_SPI_HANDLE;
#if ADS1299_BOARD_USE_UART_STREAM
extern UART_HandleTypeDef ADS1299_BOARD_UART_HANDLE;
#endif

static int example_spi_transfer(void *user, const uint8_t *tx, uint8_t *rx, size_t len)
{
    (void)user;
    if ((tx == NULL && rx == NULL) || len == 0u || len > 65535u) return -1;
    if (tx != NULL && rx != NULL) {
        return HAL_SPI_TransmitReceive(&ADS1299_BOARD_SPI_HANDLE, (uint8_t *)tx, rx,
                                       (uint16_t)len, HAL_MAX_DELAY) == HAL_OK ? 0 : -1;
    }
    if (tx != NULL) {
        return HAL_SPI_Transmit(&ADS1299_BOARD_SPI_HANDLE, (uint8_t *)tx,
                                (uint16_t)len, HAL_MAX_DELAY) == HAL_OK ? 0 : -1;
    }
    return HAL_SPI_Receive(&ADS1299_BOARD_SPI_HANDLE, rx,
                           (uint16_t)len, HAL_MAX_DELAY) == HAL_OK ? 0 : -1;
}

static void example_pin_write(void *user, ads1299_platform_pin_t pin, int level)
{
    GPIO_TypeDef *port = NULL;
    uint16_t gpio_pin = 0u;
    (void)user;
    switch (pin) {
    case ADS1299_PIN_CS:    port = ADS1299_BOARD_CS_GPIO_PORT;    gpio_pin = ADS1299_BOARD_CS_PIN; break;
    case ADS1299_PIN_RESET: port = ADS1299_BOARD_RESET_GPIO_PORT; gpio_pin = ADS1299_BOARD_RESET_PIN; break;
    case ADS1299_PIN_PWDN:  port = ADS1299_BOARD_PWDN_GPIO_PORT;  gpio_pin = ADS1299_BOARD_PWDN_PIN; break;
    case ADS1299_PIN_START: port = ADS1299_BOARD_START_GPIO_PORT; gpio_pin = ADS1299_BOARD_START_PIN; break;
    case ADS1299_PIN_DRDY:
    default: return;
    }
    HAL_GPIO_WritePin(port, gpio_pin, level ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static int example_pin_read(void *user, ads1299_platform_pin_t pin)
{
    (void)user;
    if (pin != ADS1299_PIN_DRDY) return -1;
    return HAL_GPIO_ReadPin(ADS1299_BOARD_DRDY_GPIO_PORT,
                            ADS1299_BOARD_DRDY_PIN) == GPIO_PIN_SET ? 1 : 0;
}

static void example_delay_us(void *user, uint32_t us)
{
    (void)user;
    if (us == 0u) return;
    HAL_Delay((us + 999u) / 1000u);
}

int stm32f072_ads1299_make_hal(ads1299_platform_hal_t *hal)
{
    if (hal == NULL) return -1;
    memset(hal, 0, sizeof(*hal));
    hal->spi_transfer = example_spi_transfer;
    hal->pin_write = example_pin_write;
    hal->pin_read = example_pin_read;
    hal->delay_us = example_delay_us;
    return 0;
}

int board_ads1299_hal(ads1299_platform_hal_t *hal)
{
    return stm32f072_ads1299_make_hal(hal);
}

int stm32f072_example_write(const uint8_t *data, size_t len)
{
    if (data == NULL || len == 0u || len > 65535u) return -1;
#if ADS1299_BOARD_USE_UART_STREAM
    return HAL_UART_Transmit(&ADS1299_BOARD_UART_HANDLE, (uint8_t *)data,
                             (uint16_t)len, HAL_MAX_DELAY) == HAL_OK ? 0 : -1;
#else
    (void)data; (void)len; return -1;
#endif
}

void stm32f072_example_log(const char *text)
{
    if (text == NULL) return;
#if ADS1299_BOARD_USE_UART_STREAM
    (void)stm32f072_example_write((const uint8_t *)text, strlen(text));
#else
    (void)text;
#endif
}

uint32_t stm32f072_example_timestamp_us(void)
{
    return HAL_GetTick() * 1000u;
}
