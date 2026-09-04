#include "ads1299.h"

#include <string.h>

#define ADS1299_TDECODE_DELAY_US 4u

static ads1299_status_t check_dev(const ads1299_t *dev) {
    if (!dev || !dev->port.spi_transfer || !dev->port.cs_write ||
        !dev->port.delay_us) {
        return ADS1299_EINVAL;
    }
    return ADS1299_OK;
}

static int valid_register_range(uint8_t address, size_t count) {
    if (count == 0u || address > ADS1299_REG_LAST) return 0;
    return count <= (size_t)(ADS1299_REG_LAST - address + 1u);
}

static ads1299_status_t xfer(ads1299_t *dev,
                             const uint8_t *tx,
                             uint8_t *rx,
                             size_t len) {
    if (check_dev(dev) != ADS1299_OK || len == 0u) return ADS1299_EINVAL;

    dev->port.cs_write(dev->port.user, 0);
    const int rc = dev->port.spi_transfer(dev->port.user, tx, rx, len);
    dev->port.cs_write(dev->port.user, 1);
    return rc == 0 ? ADS1299_OK : ADS1299_EIO;
}

static int transfer_register_command_byte(ads1299_t *dev, uint8_t byte) {
    const int rc = dev->port.spi_transfer(dev->port.user, &byte, NULL, 1u);
    if (rc == 0) {
        dev->port.delay_us(dev->port.user, ADS1299_TDECODE_DELAY_US);
    }
    return rc;
}

/* TI specifies RDATAC as power-up default. Always issue SDATAC before
 * command-mode register/RDATA transactions rather than trusting cached state. */
static ads1299_status_t ensure_command_mode(ads1299_t *dev) {
    ads1299_status_t rc = ads1299_command(dev, ADS1299_CMD_SDATAC);
    if (rc == ADS1299_OK) dev->continuous_mode = 0u;
    return rc;
}

ads1299_status_t ads1299_init(ads1299_t *dev, const ads1299_port_t *port) {
    if (!dev || !port || !port->spi_transfer || !port->cs_write ||
        !port->delay_us) {
        return ADS1299_EINVAL;
    }

    memset(dev, 0, sizeof(*dev));
    dev->port = *port;
    dev->port.cs_write(dev->port.user, 1);
    if (dev->port.start_write) dev->port.start_write(dev->port.user, 0);
    if (dev->port.reset_write) dev->port.reset_write(dev->port.user, 1);
    return ADS1299_OK;
}

ads1299_status_t ads1299_command(ads1299_t *dev, uint8_t command) {
    if (check_dev(dev) != ADS1299_OK) return ADS1299_EINVAL;
    /* TI states that after STANDBY only WAKEUP is a valid SPI command. */
    if (dev->standby_mode && command != ADS1299_CMD_WAKEUP) {
        return ADS1299_ESTATE;
    }

    uint8_t rx = 0;
    ads1299_status_t rc = xfer(dev, &command, &rx, 1u);
    if (rc == ADS1299_OK) {
        dev->port.delay_us(dev->port.user, 10u);
    }
    return rc;
}

ads1299_status_t ads1299_hardware_reset(ads1299_t *dev) {
    if (check_dev(dev) != ADS1299_OK || !dev->port.reset_write) {
        return ADS1299_EINVAL;
    }

    dev->port.reset_write(dev->port.user, 1);
    dev->port.delay_us(dev->port.user, 10u);
    dev->port.reset_write(dev->port.user, 0);
    dev->port.delay_us(dev->port.user, 10u);
    dev->port.reset_write(dev->port.user, 1);
    dev->port.delay_us(dev->port.user, 20u);
    dev->continuous_mode = 0u;
    dev->standby_mode = 0u;
    return ADS1299_OK;
}

ads1299_status_t ads1299_reset_command(ads1299_t *dev) {
    ads1299_status_t rc = ads1299_command(dev, ADS1299_CMD_RESET);
    if (rc == ADS1299_OK) {
        dev->port.delay_us(dev->port.user, 20u);
        dev->continuous_mode = 0u;
        dev->standby_mode = 0u;
    }
    return rc;
}

ads1299_status_t ads1299_start(ads1299_t *dev) {
    return ads1299_command(dev, ADS1299_CMD_START);
}

ads1299_status_t ads1299_stop(ads1299_t *dev) {
    return ads1299_command(dev, ADS1299_CMD_STOP);
}

ads1299_status_t ads1299_rdatac(ads1299_t *dev) {
    ads1299_status_t rc = ads1299_command(dev, ADS1299_CMD_RDATAC);
    if (rc == ADS1299_OK) dev->continuous_mode = 1u;
    return rc;
}

ads1299_status_t ads1299_sdatac(ads1299_t *dev) {
    ads1299_status_t rc = ads1299_command(dev, ADS1299_CMD_SDATAC);
    if (rc == ADS1299_OK) dev->continuous_mode = 0u;
    return rc;
}

ads1299_status_t ads1299_read_registers(ads1299_t *dev,
                                        uint8_t address,
                                        uint8_t *values,
                                        size_t count) {
    if (check_dev(dev) != ADS1299_OK || !values ||
        !valid_register_range(address, count)) {
        return ADS1299_EINVAL;
    }

    ads1299_status_t mode_rc = ensure_command_mode(dev);
    if (mode_rc != ADS1299_OK) return mode_rc;

    const uint8_t hdr[2] = {
        (uint8_t)(ADS1299_CMD_RREG | (address & 0x1Fu)),
        (uint8_t)(count - 1u),
    };
    uint8_t zeros[ADS1299_REGISTER_COUNT] = {0};

    dev->port.cs_write(dev->port.user, 0);
    if (transfer_register_command_byte(dev, hdr[0]) != 0 ||
        transfer_register_command_byte(dev, hdr[1]) != 0) {
        dev->port.cs_write(dev->port.user, 1);
        return ADS1299_EIO;
    }
    const int rc = dev->port.spi_transfer(dev->port.user, zeros, values, count);
    dev->port.cs_write(dev->port.user, 1);
    return rc == 0 ? ADS1299_OK : ADS1299_EIO;
}

ads1299_status_t ads1299_write_registers(ads1299_t *dev,
                                         uint8_t address,
                                         const uint8_t *values,
                                         size_t count) {
    if (check_dev(dev) != ADS1299_OK || !values ||
        !valid_register_range(address, count)) {
        return ADS1299_EINVAL;
    }

    for (size_t i = 0; i < count; ++i) {
        const uint8_t reg = (uint8_t)(address + i);
        if (reg == ADS1299_REG_ID || reg == ADS1299_REG_LOFF_STATP ||
            reg == ADS1299_REG_LOFF_STATN) {
            return ADS1299_EINVAL;
        }
    }

    ads1299_status_t mode_rc = ensure_command_mode(dev);
    if (mode_rc != ADS1299_OK) return mode_rc;

    const uint8_t hdr[2] = {
        (uint8_t)(ADS1299_CMD_WREG | (address & 0x1Fu)),
        (uint8_t)(count - 1u),
    };

    dev->port.cs_write(dev->port.user, 0);
    if (transfer_register_command_byte(dev, hdr[0]) != 0 ||
        transfer_register_command_byte(dev, hdr[1]) != 0 ||
        dev->port.spi_transfer(dev->port.user, values, NULL, count) != 0) {
        dev->port.cs_write(dev->port.user, 1);
        return ADS1299_EIO;
    }
    dev->port.cs_write(dev->port.user, 1);
    return ADS1299_OK;
}

ads1299_status_t ads1299_read_register(ads1299_t *dev,
                                       uint8_t address,
                                       uint8_t *value) {
    return ads1299_read_registers(dev, address, value, 1u);
}

ads1299_status_t ads1299_write_register(ads1299_t *dev,
                                        uint8_t address,
                                        uint8_t value) {
    return ads1299_write_registers(dev, address, &value, 1u);
}

int32_t ads1299_sign_extend24(uint32_t value24) {
    value24 &= 0x00FFFFFFu;
    if (value24 & 0x00800000u) value24 |= 0xFF000000u;
    return (int32_t)value24;
}

static void decode_frame(const uint8_t raw[ADS1299_FRAME_BYTES],
                         ads1299_frame_t *frame) {
    memcpy(frame->status, raw, ADS1299_STATUS_BYTES);
    for (uint8_t ch = 0; ch < ADS1299_CHANNEL_COUNT; ++ch) {
        const size_t i = ADS1299_STATUS_BYTES +
                         (size_t)ch * ADS1299_BYTES_PER_CHANNEL;
        const uint32_t u = ((uint32_t)raw[i] << 16) |
                           ((uint32_t)raw[i + 1] << 8) |
                           (uint32_t)raw[i + 2];
        frame->channel[ch] = ads1299_sign_extend24(u);
    }
}

ads1299_status_t ads1299_read_frame_continuous(ads1299_t *dev,
                                               ads1299_frame_t *frame) {
    if (check_dev(dev) != ADS1299_OK || !frame || !dev->continuous_mode) {
        return ADS1299_ESTATE;
    }

    uint8_t tx[ADS1299_FRAME_BYTES] = {0};
    uint8_t rx[ADS1299_FRAME_BYTES] = {0};
    ads1299_status_t rc = xfer(dev, tx, rx, ADS1299_FRAME_BYTES);
    if (rc == ADS1299_OK) decode_frame(rx, frame);
    return rc;
}

ads1299_status_t ads1299_read_frame_rdata(ads1299_t *dev,
                                          ads1299_frame_t *frame) {
    if (check_dev(dev) != ADS1299_OK || !frame) return ADS1299_EINVAL;

    ads1299_status_t mode_rc = ensure_command_mode(dev);
    if (mode_rc != ADS1299_OK) return mode_rc;

    const uint8_t cmd = ADS1299_CMD_RDATA;
    uint8_t raw[ADS1299_FRAME_BYTES] = {0};
    uint8_t zeros[ADS1299_FRAME_BYTES] = {0};

    dev->port.cs_write(dev->port.user, 0);
    if (dev->port.spi_transfer(dev->port.user, &cmd, NULL, 1u) != 0 ||
        dev->port.spi_transfer(dev->port.user, zeros, raw,
                               ADS1299_FRAME_BYTES) != 0) {
        dev->port.cs_write(dev->port.user, 1);
        return ADS1299_EIO;
    }
    dev->port.cs_write(dev->port.user, 1);
    decode_frame(raw, frame);
    return ADS1299_OK;
}

double ads1299_code_to_volts(int32_t code,
                             double vref_volts,
                             double gain) {
    if (gain <= 0.0 || vref_volts <= 0.0) return 0.0;
    return ((double)code * vref_volts) /
           (gain * (double)ADS1299_ADC_FULL_SCALE_CODE);
}
