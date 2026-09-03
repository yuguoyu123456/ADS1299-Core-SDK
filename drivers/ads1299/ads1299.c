#include "ads1299.h"
#include <string.h>

static ads1299_status_t check_dev(const ads1299_t *dev) {
    if (!dev || !dev->port.spi_transfer || !dev->port.cs_write || !dev->port.delay_us) {
        return ADS1299_EINVAL;
    }
    return ADS1299_OK;
}

static ads1299_status_t xfer(ads1299_t *dev, const uint8_t *tx, uint8_t *rx, size_t len) {
    if (check_dev(dev) != ADS1299_OK || !len) return ADS1299_EINVAL;
    dev->port.cs_write(dev->port.user, 0);
    int rc = dev->port.spi_transfer(dev->port.user, tx, rx, len);
    dev->port.cs_write(dev->port.user, 1);
    return rc == 0 ? ADS1299_OK : ADS1299_EIO;
}

ads1299_status_t ads1299_init(ads1299_t *dev, const ads1299_port_t *port) {
    if (!dev || !port || !port->spi_transfer || !port->cs_write || !port->delay_us) {
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
    uint8_t rx = 0;
    ads1299_status_t rc = xfer(dev, &command, &rx, 1);
    if (rc == ADS1299_OK) dev->port.delay_us(dev->port.user, 10);
    return rc;
}

ads1299_status_t ads1299_hardware_reset(ads1299_t *dev) {
    if (check_dev(dev) != ADS1299_OK || !dev->port.reset_write) return ADS1299_EINVAL;
    dev->port.reset_write(dev->port.user, 1);
    dev->port.delay_us(dev->port.user, 10);
    dev->port.reset_write(dev->port.user, 0);
    dev->port.delay_us(dev->port.user, 10);
    dev->port.reset_write(dev->port.user, 1);
    dev->port.delay_us(dev->port.user, 20);
    dev->continuous_mode = 0;
    return ADS1299_OK;
}

ads1299_status_t ads1299_reset_command(ads1299_t *dev) {
    ads1299_status_t rc = ads1299_command(dev, ADS1299_CMD_RESET);
    if (rc == ADS1299_OK) {
        dev->port.delay_us(dev->port.user, 20);
        dev->continuous_mode = 0;
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
    if (rc == ADS1299_OK) dev->continuous_mode = 1;
    return rc;
}

ads1299_status_t ads1299_sdatac(ads1299_t *dev) {
    ads1299_status_t rc = ads1299_command(dev, ADS1299_CMD_SDATAC);
    if (rc == ADS1299_OK) dev->continuous_mode = 0;
    return rc;
}

ads1299_status_t ads1299_read_registers(ads1299_t *dev, uint8_t address, uint8_t *values, size_t count) {
    if (check_dev(dev) != ADS1299_OK || !values || count == 0 || count > 256) return ADS1299_EINVAL;
    if (dev->continuous_mode) {
        ads1299_status_t rc = ads1299_sdatac(dev);
        if (rc != ADS1299_OK) return rc;
    }

    uint8_t hdr[2] = {(uint8_t)(ADS1299_CMD_RREG | (address & 0x1Fu)), (uint8_t)(count - 1u)};
    dev->port.cs_write(dev->port.user, 0);
    if (dev->port.spi_transfer(dev->port.user, hdr, NULL, sizeof(hdr)) != 0) {
        dev->port.cs_write(dev->port.user, 1);
        return ADS1299_EIO;
    }
    uint8_t zeros[256] = {0};
    int rc = dev->port.spi_transfer(dev->port.user, zeros, values, count);
    dev->port.cs_write(dev->port.user, 1);
    return rc == 0 ? ADS1299_OK : ADS1299_EIO;
}

ads1299_status_t ads1299_write_registers(ads1299_t *dev, uint8_t address, const uint8_t *values, size_t count) {
    if (check_dev(dev) != ADS1299_OK || !values || count == 0 || count > 256) return ADS1299_EINVAL;
    if (dev->continuous_mode) {
        ads1299_status_t rc = ads1299_sdatac(dev);
        if (rc != ADS1299_OK) return rc;
    }

    uint8_t hdr[2] = {(uint8_t)(ADS1299_CMD_WREG | (address & 0x1Fu)), (uint8_t)(count - 1u)};
    dev->port.cs_write(dev->port.user, 0);
    if (dev->port.spi_transfer(dev->port.user, hdr, NULL, sizeof(hdr)) != 0 ||
        dev->port.spi_transfer(dev->port.user, values, NULL, count) != 0) {
        dev->port.cs_write(dev->port.user, 1);
        return ADS1299_EIO;
    }
    dev->port.cs_write(dev->port.user, 1);
    return ADS1299_OK;
}

ads1299_status_t ads1299_read_register(ads1299_t *dev, uint8_t address, uint8_t *value) {
    return ads1299_read_registers(dev, address, value, 1);
}

ads1299_status_t ads1299_write_register(ads1299_t *dev, uint8_t address, uint8_t value) {
    return ads1299_write_registers(dev, address, &value, 1);
}

ads1299_status_t ads1299_set_data_rate(ads1299_t *dev, uint8_t dr_code) {
    uint8_t config1 = 0;
    ads1299_status_t rc = ads1299_read_register(dev, ADS1299_REG_CONFIG1, &config1);
    if (rc != ADS1299_OK) return rc;
    config1 = (uint8_t)((config1 & ~0x07u) | (dr_code & 0x07u));
    return ads1299_write_register(dev, ADS1299_REG_CONFIG1, config1);
}

ads1299_status_t ads1299_set_channel(ads1299_t *dev, uint8_t channel_1_to_8,
                                    uint8_t gain_code, uint8_t mux_code,
                                    int srb2, int power_down) {
    if (channel_1_to_8 < 1 || channel_1_to_8 > 8) return ADS1299_EINVAL;
    uint8_t value = (uint8_t)((gain_code & ADS1299_CH_GAIN_MASK) |
                              (mux_code & ADS1299_CH_MUX_MASK));
    if (srb2) value |= ADS1299_CH_SRB2;
    if (power_down) value |= ADS1299_CH_POWER_DOWN;
    return ads1299_write_register(dev,
                                  (uint8_t)(ADS1299_REG_CH1SET + channel_1_to_8 - 1u),
                                  value);
}

int32_t ads1299_sign_extend24(uint32_t value24) {
    value24 &= 0x00FFFFFFu;
    if (value24 & 0x00800000u) value24 |= 0xFF000000u;
    return (int32_t)value24;
}

static void decode_frame(const uint8_t raw[ADS1299_FRAME_BYTES], ads1299_frame_t *frame) {
    memcpy(frame->status, raw, ADS1299_STATUS_BYTES);
    for (uint8_t ch = 0; ch < ADS1299_CHANNEL_COUNT; ++ch) {
        size_t i = ADS1299_STATUS_BYTES + (size_t)ch * ADS1299_BYTES_PER_CHANNEL;
        uint32_t u = ((uint32_t)raw[i] << 16) |
                     ((uint32_t)raw[i + 1] << 8) |
                     (uint32_t)raw[i + 2];
        frame->channel[ch] = ads1299_sign_extend24(u);
    }
}

ads1299_status_t ads1299_read_frame_continuous(ads1299_t *dev, ads1299_frame_t *frame) {
    if (check_dev(dev) != ADS1299_OK || !frame || !dev->continuous_mode) return ADS1299_EINVAL;
    uint8_t tx[ADS1299_FRAME_BYTES] = {0};
    uint8_t rx[ADS1299_FRAME_BYTES] = {0};
    ads1299_status_t rc = xfer(dev, tx, rx, ADS1299_FRAME_BYTES);
    if (rc == ADS1299_OK) decode_frame(rx, frame);
    return rc;
}

ads1299_status_t ads1299_read_frame_rdata(ads1299_t *dev, ads1299_frame_t *frame) {
    if (check_dev(dev) != ADS1299_OK || !frame || dev->continuous_mode) return ADS1299_EINVAL;
    uint8_t cmd = ADS1299_CMD_RDATA;
    uint8_t raw[ADS1299_FRAME_BYTES] = {0};
    uint8_t zeros[ADS1299_FRAME_BYTES] = {0};

    dev->port.cs_write(dev->port.user, 0);
    if (dev->port.spi_transfer(dev->port.user, &cmd, NULL, 1) != 0 ||
        dev->port.spi_transfer(dev->port.user, zeros, raw, ADS1299_FRAME_BYTES) != 0) {
        dev->port.cs_write(dev->port.user, 1);
        return ADS1299_EIO;
    }
    dev->port.cs_write(dev->port.user, 1);
    decode_frame(raw, frame);
    return ADS1299_OK;
}

double ads1299_code_to_volts(int32_t code, double vref_volts, double gain) {
    if (gain <= 0.0) return 0.0;
    return ((double)code * vref_volts) / (gain * 8388607.0);
}
