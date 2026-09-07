#include "ads1299_mcuxpresso_init.h"

int ads1299_mcuxpresso_init(ads1299_platform_hal_t *hal,
                          ads1299_mcuxpresso_ctx_t *ctx,
                          uint32_t source_clock_hz, uint32_t spi_clock_hz) {
    ads1299_platform_hal_t candidate;
    if (!hal || source_clock_hz == 0u || spi_clock_hz == 0u ||
        spi_clock_hz > source_clock_hz / 2u ||
        ads1299_mcuxpresso_make_hal(&candidate, ctx) != 0) return -1;

    /* CS high before enabling SPI. Keep START low, RESET/PWDN inactive.
     * The Core owns subsequent reset timing and conversion sequencing. */
    gpio_pin_config_t output = {kGPIO_DigitalOutput, 1u, kGPIO_NoIntmode};
    GPIO_PinInit(ctx->cs.base, ctx->cs.pin, &output);
    GPIO_PinInit(ctx->reset.base, ctx->reset.pin, &output);
    GPIO_PinInit(ctx->pwdn.base, ctx->pwdn.pin, &output);
    output.outputLogic = 0u;
    GPIO_PinInit(ctx->start.base, ctx->start.pin, &output);
    gpio_pin_config_t input = {kGPIO_DigitalInput, 0u, kGPIO_NoIntmode};
    GPIO_PinInit(ctx->drdy.base, ctx->drdy.pin, &input);

    lpspi_master_config_t config;
    LPSPI_MasterGetDefaultConfig(&config);
    config.baudRate = spi_clock_hz;
    config.bitsPerFrame = 8u;
    config.cpol = kLPSPI_ClockPolarityActiveHigh;
    config.cpha = kLPSPI_ClockPhaseSecondEdge;
    config.direction = kLPSPI_MsbFirst;
    /* No PCS pin may be muxed to LPSPI. External CS is a GPIO. */
    config.whichPcs = kLPSPI_Pcs0;
    LPSPI_MasterInit(ctx->lpspi, &config, source_clock_hz);
    ctx->transfer_flags = kLPSPI_MasterPcs0;
    *hal = candidate;
    return 0;
}
