#include "ads1299_lpc54628_mcuxpresso.h"

static int gpio_valid(const ads1299_lpc54628_gpio_t *p) {
    return p != 0 && p->gpio != 0;
}

static ads1299_lpc54628_gpio_t *pin_for(ads1299_lpc54628_hw_t *hw,
                                        ads1299_platform_pin_t pin) {
    switch (pin) {
    case ADS1299_PIN_CS: return &hw->cs;
    case ADS1299_PIN_RESET: return &hw->reset;
    case ADS1299_PIN_PWDN: return &hw->pwdn;
    case ADS1299_PIN_START: return &hw->start;
    case ADS1299_PIN_DRDY: return &hw->drdy;
    default: return 0;
    }
}

static int mcux_spi_transfer(void *user, const uint8_t *tx, uint8_t *rx,
                             size_t len) {
    ads1299_lpc54628_hw_t *hw = (ads1299_lpc54628_hw_t *)user;
    spi_transfer_t xfer;
    status_t status;

    if (hw == 0 || hw->spi == 0 || len == 0u) return -1;
    xfer.txData = (uint8_t *)tx;
    xfer.rxData = rx;
    xfer.dataSize = len;
    xfer.configFlags = kSPI_FrameAssert;
    status = SPI_MasterTransferBlocking(hw->spi, &xfer);
    return status == kStatus_Success ? 0 : -1;
}

static void mcux_pin_write(void *user, ads1299_platform_pin_t pin, int level) {
    ads1299_lpc54628_hw_t *hw = (ads1299_lpc54628_hw_t *)user;
    ads1299_lpc54628_gpio_t *p = pin_for(hw, pin);
    if (p != 0 && gpio_valid(p)) GPIO_PinWrite(p->gpio, p->port, p->pin, level ? 1u : 0u);
}

static int mcux_pin_read(void *user, ads1299_platform_pin_t pin) {
    ads1299_lpc54628_hw_t *hw = (ads1299_lpc54628_hw_t *)user;
    ads1299_lpc54628_gpio_t *p = pin_for(hw, pin);
    if (p == 0 || !gpio_valid(p)) return 1;
    return GPIO_PinRead(p->gpio, p->port, p->pin) ? 1 : 0;
}

static void mcux_delay_us(void *user, uint32_t us) {
    ads1299_lpc54628_hw_t *hw = (ads1299_lpc54628_hw_t *)user;
    if (hw != 0 && hw->delay_us != 0) hw->delay_us(us);
}

static void gpio_output(const ads1299_lpc54628_gpio_t *p, uint8_t initial) {
    gpio_pin_config_t cfg = { kGPIO_DigitalOutput, initial };
    GPIO_PinInit(p->gpio, p->port, p->pin, &cfg);
}

int ads1299_lpc54628_mcuxpresso_init(ads1299_lpc54628_hw_t *hw,
                                     ads1299_platform_hal_t *hal) {
    spi_master_config_t cfg;
    gpio_pin_config_t input = { kGPIO_DigitalInput, 0u };

    if (hw == 0 || hal == 0 || hw->spi == 0 || hw->delay_us == 0) return -1;
    if (!gpio_valid(&hw->cs) || !gpio_valid(&hw->reset) ||
        !gpio_valid(&hw->pwdn) || !gpio_valid(&hw->start) ||
        !gpio_valid(&hw->drdy)) return -2;
    if (hw->spi_src_clock_hz == 0u || hw->baud_hz == 0u || hw->baud_hz > 4000000u)
        return -3;

    SPI_MasterGetDefaultConfig(&cfg);
    cfg.baudRate_Bps = hw->baud_hz;
    cfg.polarity = kSPI_ClockPolarityActiveHigh;
    cfg.phase = kSPI_ClockPhaseSecondEdge;
    cfg.direction = kSPI_MsbFirst;
    if (SPI_MasterInit(hw->spi, &cfg, hw->spi_src_clock_hz) != kStatus_Success)
        return -4;

    gpio_output(&hw->cs, 1u);
    gpio_output(&hw->reset, 1u);
    gpio_output(&hw->pwdn, 1u);
    gpio_output(&hw->start, 0u);
    GPIO_PinInit(hw->drdy.gpio, hw->drdy.port, hw->drdy.pin, &input);

    hal->user = hw;
    hal->spi_transfer = mcux_spi_transfer;
    hal->pin_write = mcux_pin_write;
    hal->pin_read = mcux_pin_read;
    hal->delay_us = mcux_delay_us;
    return 0;
}
