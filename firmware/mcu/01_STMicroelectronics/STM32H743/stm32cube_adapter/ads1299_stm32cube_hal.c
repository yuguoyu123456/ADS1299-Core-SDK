#include "ads1299_stm32cube_hal.h"

#include <limits.h>
#include <string.h>

#define ADS1299_STM32CUBE_RX_CHUNK 32u

static int transfer_chunk(SPI_HandleTypeDef *spi,
                          const uint8_t *tx,
                          uint8_t *rx,
                          uint16_t length,
                          uint32_t timeout_ms) {
    HAL_StatusTypeDef status;
    if (tx && rx) {
        status = HAL_SPI_TransmitReceive(spi, (uint8_t *)tx, rx, length, timeout_ms);
    } else if (tx) {
        status = HAL_SPI_Transmit(spi, (uint8_t *)tx, length, timeout_ms);
    } else {
        uint8_t zeros[ADS1299_STM32CUBE_RX_CHUNK] = {0};
        uint16_t offset = 0u;
        while (offset < length) {
            uint16_t part = (uint16_t)(length - offset);
            if (part > ADS1299_STM32CUBE_RX_CHUNK) {
                part = ADS1299_STM32CUBE_RX_CHUNK;
            }
            status = HAL_SPI_TransmitReceive(spi, zeros, &rx[offset], part, timeout_ms);
            if (status != HAL_OK) return -1;
            offset = (uint16_t)(offset + part);
        }
        return 0;
    }
    return status == HAL_OK ? 0 : -1;
}

static int cube_spi_transfer(void *user,
                             const uint8_t *tx,
                             uint8_t *rx,
                             size_t length) {
    ads1299_stm32cube_context_t *context = user;
    size_t offset = 0u;
    if (!context || !context->spi || length == 0u || (!tx && !rx)) return -1;

    while (offset < length) {
        size_t remaining = length - offset;
        uint16_t part = remaining > UINT16_MAX ? UINT16_MAX : (uint16_t)remaining;
        if (transfer_chunk(context->spi,
                           tx ? &tx[offset] : NULL,
                           rx ? &rx[offset] : NULL,
                           part,
                           context->spi_timeout_ms) != 0) {
            return -1;
        }
        offset += part;
    }
    return 0;
}

static void cube_pin_write(void *user, ads1299_platform_pin_t pin, int level) {
    ads1299_stm32cube_context_t *context = user;
    if (!context || pin > ADS1299_PIN_DRDY || !context->gpio[pin].port) return;
    HAL_GPIO_WritePin(context->gpio[pin].port,
                      context->gpio[pin].pin,
                      level ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static int cube_pin_read(void *user, ads1299_platform_pin_t pin) {
    ads1299_stm32cube_context_t *context = user;
    if (!context || pin > ADS1299_PIN_DRDY || !context->gpio[pin].port) return 1;
    return HAL_GPIO_ReadPin(context->gpio[pin].port, context->gpio[pin].pin) ==
                   GPIO_PIN_SET
               ? 1
               : 0;
}

static void cube_delay_us(void *user, uint32_t microseconds) {
    uint32_t cycles_per_us;
    (void)user;
    if (microseconds == 0u) return;

    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    cycles_per_us = SystemCoreClock / 1000000u;
    if (cycles_per_us == 0u) cycles_per_us = 1u;

    while (microseconds != 0u) {
        const uint32_t chunk_us = microseconds > 1000000u ? 1000000u : microseconds;
        const uint32_t start = DWT->CYCCNT;
        const uint32_t cycles = cycles_per_us * chunk_us;
        while ((uint32_t)(DWT->CYCCNT - start) < cycles) {
            __NOP();
        }
        microseconds -= chunk_us;
    }
}

int ads1299_stm32cube_make_hal(ads1299_platform_hal_t *hal,
                               ads1299_stm32cube_context_t *context) {
    size_t pin;
    if (!hal || !context || !context->spi || context->spi_timeout_ms == 0u) return -1;
    for (pin = 0u; pin < 5u; ++pin) {
        if (!context->gpio[pin].port || context->gpio[pin].pin == 0u) return -1;
    }
    hal->user = context;
    hal->spi_transfer = cube_spi_transfer;
    hal->pin_write = cube_pin_write;
    hal->pin_read = cube_pin_read;
    hal->delay_us = cube_delay_us;
    return 0;
}
