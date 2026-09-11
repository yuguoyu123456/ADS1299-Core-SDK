#include <string.h>

#include "ads1299_spi.h"
#include "../board/ads1299_board_config.h"

/* CubeMX normally generates these GPIO aliases in main.h. */
#include "main.h"

static GPIO_TypeDef *board_port_for_pin(ads1299_platform_pin_t pin)
{
    switch (pin) {
    case ADS1299_PIN_CS:    return ADS1299_CS_GPIO_Port;
    case ADS1299_PIN_RESET: return ADS1299_RESET_GPIO_Port;
    case ADS1299_PIN_PWDN:  return ADS1299_PWDN_GPIO_Port;
    case ADS1299_PIN_START: return ADS1299_START_GPIO_Port;
    case ADS1299_PIN_DRDY:  return ADS1299_DRDY_GPIO_Port;
    default:                return NULL;
    }
}

static uint16_t board_mask_for_pin(ads1299_platform_pin_t pin)
{
    switch (pin) {
    case ADS1299_PIN_CS:    return ADS1299_CS_Pin;
    case ADS1299_PIN_RESET: return ADS1299_RESET_Pin;
    case ADS1299_PIN_PWDN:  return ADS1299_PWDN_Pin;
    case ADS1299_PIN_START: return ADS1299_START_Pin;
    case ADS1299_PIN_DRDY:  return ADS1299_DRDY_Pin;
    default:                return 0u;
    }
}

static int board_spi_transfer(void *user,
                              const uint8_t *tx,
                              uint8_t *rx,
                              size_t len)
{
    SPI_HandleTypeDef *spi = (SPI_HandleTypeDef *)user;
    uint8_t tx_scratch[32];
    uint8_t rx_scratch[32];
    size_t offset = 0u;

    if (spi == NULL || len == 0u || (tx == NULL && rx == NULL)) {
        return -1;
    }

    while (offset < len) {
        const size_t remaining = len - offset;
        const uint16_t chunk = (uint16_t)(remaining > sizeof(tx_scratch)
                                              ? sizeof(tx_scratch)
                                              : remaining);
        uint8_t *tx_ptr = tx_scratch;
        uint8_t *rx_ptr = rx_scratch;

        if (tx != NULL) {
            memcpy(tx_scratch, tx + offset, chunk);
        } else {
            memset(tx_scratch, 0, chunk);
        }

        if (HAL_SPI_TransmitReceive(spi,
                                    tx_ptr,
                                    rx_ptr,
                                    chunk,
                                    ADS1299_BOARD_SPI_TIMEOUT_MS) != HAL_OK) {
            return -1;
        }

        if (rx != NULL) {
            memcpy(rx + offset, rx_scratch, chunk);
        }
        offset += chunk;
    }

    return 0;
}

static void board_pin_write(void *user, ads1299_platform_pin_t pin, int level)
{
    GPIO_TypeDef *port;
    uint16_t mask;
    (void)user;

    port = board_port_for_pin(pin);
    mask = board_mask_for_pin(pin);
    if (port == NULL || mask == 0u || pin == ADS1299_PIN_DRDY) {
        return;
    }

    HAL_GPIO_WritePin(port, mask, level ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static int board_pin_read(void *user, ads1299_platform_pin_t pin)
{
    GPIO_TypeDef *port;
    uint16_t mask;
    (void)user;

    port = board_port_for_pin(pin);
    mask = board_mask_for_pin(pin);
    if (port == NULL || mask == 0u) {
        return 1;
    }

    return HAL_GPIO_ReadPin(port, mask) == GPIO_PIN_SET ? 1 : 0;
}

static void board_delay_us(void *user, uint32_t us)
{
    uint32_t start;
    uint32_t ticks;
    (void)user;

    if (us == 0u) {
        return;
    }

    /* Cortex-M7 DWT cycle counter gives a board-independent microsecond delay
     * once SystemCoreClock is correct.  Cube startup owns the clock tree. */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    start = DWT->CYCCNT;
    ticks = (SystemCoreClock / 1000000u) * us;
    while ((uint32_t)(DWT->CYCCNT - start) < ticks) {
        __NOP();
    }
}

int board_ads1299_hal(ads1299_platform_hal_t *hal)
{
    if (hal == NULL) {
        return -1;
    }

    hal->user = ADS1299_BOARD_SPI_HANDLE;
    hal->spi_transfer = board_spi_transfer;
    hal->pin_write = board_pin_write;
    hal->pin_read = board_pin_read;
    hal->delay_us = board_delay_us;
    return 0;
}
