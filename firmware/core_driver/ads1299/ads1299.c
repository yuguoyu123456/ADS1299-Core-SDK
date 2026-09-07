/* Consolidated portable ADS1299 device/control/runtime implementation. */

/* ===== ads1299.c ===== */
#include "ads1299.h"
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

ads1299_status_t ads1299_read_frame_continuous(ads1299_t *dev,
                                               ads1299_frame_t *frame) {
    if (check_dev(dev) != ADS1299_OK || !frame || !dev->continuous_mode) {
        return ADS1299_ESTATE;
    }

    const ads1299_variant_t variant = ads1299_effective_variant(dev);
    const size_t frame_bytes = ads1299_frame_bytes_for_variant(variant);
    uint8_t tx[ADS1299_FRAME_BYTES] = {0};
    uint8_t rx[ADS1299_FRAME_BYTES] = {0};
    ads1299_status_t rc = xfer(dev, tx, rx, frame_bytes);
    if (rc != ADS1299_OK) return rc;
    return ads1299_decode_frame_variant(rx, frame_bytes, variant, frame, NULL);
}

ads1299_status_t ads1299_read_frame_rdata(ads1299_t *dev,
                                          ads1299_frame_t *frame) {
    if (check_dev(dev) != ADS1299_OK || !frame) return ADS1299_EINVAL;

    ads1299_status_t mode_rc = ensure_command_mode(dev);
    if (mode_rc != ADS1299_OK) return mode_rc;

    const ads1299_variant_t variant = ads1299_effective_variant(dev);
    const size_t frame_bytes = ads1299_frame_bytes_for_variant(variant);
    const uint8_t cmd = ADS1299_CMD_RDATA;
    uint8_t raw[ADS1299_FRAME_BYTES] = {0};
    uint8_t zeros[ADS1299_FRAME_BYTES] = {0};

    dev->port.cs_write(dev->port.user, 0);
    if (dev->port.spi_transfer(dev->port.user, &cmd, NULL, 1u) != 0 ||
        dev->port.spi_transfer(dev->port.user, zeros, raw,
                               frame_bytes) != 0) {
        dev->port.cs_write(dev->port.user, 1);
        return ADS1299_EIO;
    }
    dev->port.cs_write(dev->port.user, 1);
    return ads1299_decode_frame_variant(raw, frame_bytes, variant, frame, NULL);
}

double ads1299_code_to_volts(int32_t code,
                             double vref_volts,
                             double gain) {
    if (gain <= 0.0 || vref_volts <= 0.0) return 0.0;
    return ((double)code * vref_volts) /
           (gain * (double)ADS1299_ADC_FULL_SCALE_CODE);
}

/* ===== bringup ===== */
#include "ads1299.h"
#include "ads1299.h"

static uint8_t active_channel_count(const ads1299_t *dev) {
    if (dev && (dev->channel_count == 4u || dev->channel_count == 6u ||
                dev->channel_count == 8u)) {
        return dev->channel_count;
    }
    return ADS1299_CHANNEL_COUNT;
}

static int encode_config2_field(ads1299_field_id_t field,
                                uint8_t code,
                                uint8_t *value) {
    uint8_t next = 0u;
    if (!value || ads1299_field_encode(field, 0u, code, *value,
                                       ADS1299_VARIANT_8CH, &next) != 0) {
        return -1;
    }
    *value = next;
    return 0;
}

ads1299_status_t ads1299_set_daisy_chain_mode(ads1299_t *dev, int enable) {
    if (!dev) return ADS1299_EINVAL;
    const ads1299_variant_t variant = ads1299_effective_variant(dev);
    /* TI DAISY_EN is inverted: 0=daisy-chain, 1=multiple readback. */
    return ads1299_safe_write_field(dev, ADS1299_FIELD_CONFIG1_DAISY_EN, 0u,
                                    enable ? 0u : 1u, variant, NULL);
}

ads1299_status_t ads1299_build_test_config2(
    const ads1299_test_signal_config_t *config, uint8_t *value) {
    if (!config || !value) return ADS1299_EINVAL;

    uint8_t built = ADS1299_RESET_CONFIG2;
    if (encode_config2_field(ADS1299_FIELD_CONFIG2_INT_CAL,
                             config->use_internal_source, &built) != 0 ||
        encode_config2_field(ADS1299_FIELD_CONFIG2_CAL_AMP,
                             config->amplitude_x2, &built) != 0 ||
        encode_config2_field(ADS1299_FIELD_CONFIG2_CAL_FREQ,
                             config->frequency_code, &built) != 0) {
        return ADS1299_EINVAL;
    }
    *value = built;
    return ADS1299_OK;
}

uint8_t ads1299_make_test_config2(const ads1299_test_signal_config_t *config) {
    uint8_t value = ADS1299_CONFIG2_RESERVED_BASE;
    if (ads1299_build_test_config2(config, &value) != ADS1299_OK) {
        return ADS1299_CONFIG2_RESERVED_BASE;
    }
    return value;
}

ads1299_status_t ads1299_configure_test_signal(
    ads1299_t *dev, const ads1299_test_signal_config_t *config) {
    if (!dev || !config) return ADS1299_EINVAL;
    uint8_t value = 0u;
    ads1299_status_t rc = ads1299_build_test_config2(config, &value);
    if (rc != ADS1299_OK) return rc;
    return ads1299_strict_write_register(dev, ADS1299_REG_CONFIG2, value,
                                         ads1299_effective_variant(dev));
}

uint8_t ads1299_make_internal_test_config2(int amplitude_x2, uint8_t freq_code) {
    const ads1299_test_signal_config_t config = {
        .use_internal_source = 1u,
        .amplitude_x2 = amplitude_x2 ? 1u : 0u,
        .frequency_code = freq_code,
    };
    return ads1299_make_test_config2(&config);
}

ads1299_status_t ads1299_configure_internal_test(ads1299_t *dev,
                                                 uint8_t gain_code,
                                                 int amplitude_x2,
                                                 uint8_t freq_code) {
    const ads1299_test_signal_config_t config = {
        .use_internal_source = 1u,
        .amplitude_x2 = amplitude_x2 ? 1u : 0u,
        .frequency_code = freq_code,
    };
    ads1299_status_t rc = ads1299_configure_test_signal(dev, &config);
    if (rc != ADS1299_OK) return rc;

    const uint8_t count = active_channel_count(dev);
    for (uint8_t ch = 1; ch <= count; ++ch) {
        rc = ads1299_set_channel(dev, ch, gain_code, ADS1299_MUX_TEST, 0, 0);
        if (rc != ADS1299_OK) return rc;
    }
    return ADS1299_OK;
}

ads1299_status_t ads1299_configure_input_short_test(ads1299_t *dev,
                                                    uint8_t gain_code) {
    if (!dev) return ADS1299_EINVAL;
    ads1299_status_t rc;
    const uint8_t count = active_channel_count(dev);
    for (uint8_t ch = 1; ch <= count; ++ch) {
        rc = ads1299_set_channel(dev, ch, gain_code, ADS1299_MUX_SHORTED, 0, 0);
        if (rc != ADS1299_OK) return rc;
    }
    return ADS1299_OK;
}

/* ===== control ===== */
#include "ads1299.h"
#include "ads1299.h"

static uint8_t device_channel_mask(const ads1299_t *dev) {
    return ads1299_variant_channel_mask(ads1299_effective_variant(dev));
}

static int valid_channel(const ads1299_t *dev, uint8_t channel_1_to_8) {
    if (!dev || channel_1_to_8 < 1u || channel_1_to_8 > 8u) return 0;
    const uint8_t mask = device_channel_mask(dev);
    return (mask & ADS1299_CHANNEL_MASK(channel_1_to_8)) != 0u;
}

static int encode_field(ads1299_field_id_t field,
                        uint8_t channel_1_to_8,
                        uint8_t code,
                        ads1299_variant_t variant,
                        uint8_t *register_value) {
    uint8_t next = 0u;
    if (!register_value ||
        ads1299_field_encode(field, channel_1_to_8, code, *register_value,
                             variant, &next) != 0) {
        return -1;
    }
    *register_value = next;
    return 0;
}

static int gain_bits_to_field_code(uint8_t gain_bits, uint8_t *field_code) {
    if (!field_code || (gain_bits & (uint8_t)~ADS1299_CH_GAIN_MASK) != 0u) return -1;
    const uint8_t code = (uint8_t)((gain_bits & ADS1299_CH_GAIN_MASK) >> 4u);
    if (!ads1299_field_code_valid(ADS1299_FIELD_CH_GAIN, code,
                                  ADS1299_VARIANT_8CH)) {
        return -1;
    }
    *field_code = code;
    return 0;
}

ads1299_status_t ads1299_update_register_bits(ads1299_t *dev,
                                              uint8_t address,
                                              uint8_t mask,
                                              uint8_t value) {
    if (!dev || address > ADS1299_REG_LAST) return ADS1299_EINVAL;

    uint8_t current = 0;
    ads1299_status_t rc = ads1299_read_register(dev, address, &current);
    if (rc != ADS1299_OK) return rc;

    const uint8_t next = (uint8_t)((current & (uint8_t)~mask) | (value & mask));
    if (next == current) return ADS1299_OK;
    return ads1299_write_register(dev, address, next);
}

ads1299_status_t ads1299_verify_register(ads1299_t *dev,
                                         uint8_t address,
                                         uint8_t expected,
                                         uint8_t mask,
                                         uint8_t *actual) {
    if (!dev || address > ADS1299_REG_LAST) return ADS1299_EINVAL;

    uint8_t value = 0;
    ads1299_status_t rc = ads1299_read_register(dev, address, &value);
    if (actual) *actual = value;
    if (rc != ADS1299_OK) return rc;

    return ((value & mask) == (expected & mask)) ? ADS1299_OK : ADS1299_EVERIFY;
}

ads1299_status_t ads1299_read_device_id(ads1299_t *dev,
                                        ads1299_device_id_t *id) {
    if (!dev || !id) return ADS1299_EINVAL;

    uint8_t raw = 0;
    ads1299_status_t rc = ads1299_read_register(dev, ADS1299_REG_ID, &raw);
    if (rc != ADS1299_OK) return rc;

    id->raw = raw;
    id->revision = (uint8_t)((raw & ADS1299_ID_REV_MASK) >> ADS1299_ID_REV_SHIFT);
    id->device_code = (uint8_t)((raw & ADS1299_ID_DEVICE_MASK) >> ADS1299_ID_DEVICE_SHIFT);
    id->channel_code = (uint8_t)(raw & ADS1299_ID_CHANNEL_MASK);
    id->is_ads1299_family = (uint8_t)(
        ((raw & ADS1299_ID_RESERVED_ONE) != 0u) &&
        (id->device_code == ADS1299_ID_DEVICE_ADS1299));

    switch (id->channel_code) {
        case ADS1299_ID_CHANNEL_4: id->channel_count = 4u; break;
        case ADS1299_ID_CHANNEL_6: id->channel_count = 6u; break;
        case ADS1299_ID_CHANNEL_8: id->channel_count = 8u; break;
        default: id->channel_count = 0u; break;
    }

    dev->channel_count = id->is_ads1299_family ? id->channel_count : 0u;
    return ADS1299_OK;
}

ads1299_status_t ads1299_read_register_dump(ads1299_t *dev,
                                            ads1299_register_dump_t *dump) {
    if (!dev || !dump) return ADS1299_EINVAL;
    return ads1299_read_registers(dev, ADS1299_REG_ID, dump->value,
                                  ADS1299_REGISTER_COUNT);
}

ads1299_status_t ads1299_set_data_rate(ads1299_t *dev, uint8_t dr_code) {
    if (!dev) return ADS1299_EINVAL;
    const ads1299_variant_t variant = ads1299_effective_variant(dev);
    return ads1299_safe_write_field(dev, ADS1299_FIELD_CONFIG1_DR, 0u, dr_code,
                                    variant, NULL);
}

ads1299_status_t ads1299_set_clock_output(ads1299_t *dev, int enable) {
    if (!dev) return ADS1299_EINVAL;
    const ads1299_variant_t variant = ads1299_effective_variant(dev);
    return ads1299_safe_write_field(dev, ADS1299_FIELD_CONFIG1_CLK_EN, 0u,
                                    enable ? 1u : 0u, variant, NULL);
}

ads1299_status_t ads1299_set_single_shot_mode(ads1299_t *dev, int enable) {
    if (!dev) return ADS1299_EINVAL;
    const ads1299_variant_t variant = ads1299_effective_variant(dev);
    return ads1299_safe_write_field(dev, ADS1299_FIELD_CONFIG4_SINGLE_SHOT, 0u,
                                    enable ? 1u : 0u, variant, NULL);
}

ads1299_status_t ads1299_set_channel(ads1299_t *dev,
                                     uint8_t channel_1_to_8,
                                     uint8_t gain_code,
                                     uint8_t mux_code,
                                     int srb2,
                                     int power_down) {
    if (!valid_channel(dev, channel_1_to_8)) return ADS1299_EINVAL;

    uint8_t gain_field = 0u;
    if (gain_bits_to_field_code(gain_code, &gain_field) != 0) return ADS1299_EINVAL;

    const ads1299_variant_t variant = ads1299_effective_variant(dev);
    if (!ads1299_field_code_valid(ADS1299_FIELD_CH_MUX, mux_code, variant)) {
        return ADS1299_EINVAL;
    }

    uint8_t address = 0u;
    if (ads1299_field_register_address(ADS1299_FIELD_CH_GAIN, channel_1_to_8,
                                       variant, &address) != 0) {
        return ADS1299_EINVAL;
    }

    uint8_t value = ADS1299_RESET_CHNSET;
    if (encode_field(ADS1299_FIELD_CH_POWER_DOWN, channel_1_to_8,
                     power_down ? 1u : 0u, variant, &value) != 0 ||
        encode_field(ADS1299_FIELD_CH_GAIN, channel_1_to_8, gain_field,
                     variant, &value) != 0 ||
        encode_field(ADS1299_FIELD_CH_SRB2, channel_1_to_8,
                     srb2 ? 1u : 0u, variant, &value) != 0 ||
        encode_field(ADS1299_FIELD_CH_MUX, channel_1_to_8, mux_code,
                     variant, &value) != 0) {
        return ADS1299_EINVAL;
    }
    return ads1299_strict_write_register(dev, address, value, variant);
}

ads1299_status_t ads1299_power_down_channel(ads1299_t *dev,
                                            uint8_t channel_1_to_8) {
    if (!valid_channel(dev, channel_1_to_8)) return ADS1299_EINVAL;
    return ads1299_set_channel(dev, channel_1_to_8, ADS1299_GAIN_1,
                               ADS1299_MUX_SHORTED, 0, 1);
}

ads1299_status_t ads1299_set_srb1(ads1299_t *dev, int enable) {
    if (!dev) return ADS1299_EINVAL;
    const ads1299_variant_t variant = ads1299_effective_variant(dev);
    return ads1299_safe_write_field(dev, ADS1299_FIELD_MISC1_SRB1, 0u,
                                    enable ? 1u : 0u, variant, NULL);
}

ads1299_status_t ads1299_set_channel_srb2(ads1299_t *dev,
                                          uint8_t channel_1_to_8,
                                          int enable) {
    if (!valid_channel(dev, channel_1_to_8)) return ADS1299_EINVAL;
    const ads1299_variant_t variant = ads1299_effective_variant(dev);
    return ads1299_safe_write_field(dev, ADS1299_FIELD_CH_SRB2, channel_1_to_8,
                                    enable ? 1u : 0u, variant, NULL);
}

ads1299_status_t ads1299_configure_bias(ads1299_t *dev,
                                        const ads1299_bias_config_t *config) {
    if (!dev || !config) return ADS1299_EINVAL;

    const ads1299_variant_t variant = ads1299_effective_variant(dev);
    const uint8_t chmask = ads1299_variant_channel_mask(variant);
    const uint8_t positive_mask = (uint8_t)(config->positive_sense_mask & chmask);
    const uint8_t negative_mask = (uint8_t)(config->negative_sense_mask & chmask);

    uint8_t config3 = ADS1299_RESET_CONFIG3;
    if (encode_field(ADS1299_FIELD_CONFIG3_PD_REFBUF, 0u,
                     config->enable_reference_buffer ? 1u : 0u,
                     variant, &config3) != 0 ||
        encode_field(ADS1299_FIELD_CONFIG3_BIAS_MEAS, 0u,
                     config->enable_bias_measurement ? 1u : 0u,
                     variant, &config3) != 0 ||
        encode_field(ADS1299_FIELD_CONFIG3_BIASREF_INT, 0u,
                     config->use_internal_bias_reference ? 1u : 0u,
                     variant, &config3) != 0 ||
        encode_field(ADS1299_FIELD_CONFIG3_PD_BIAS, 0u,
                     config->enable_bias_buffer ? 1u : 0u,
                     variant, &config3) != 0 ||
        encode_field(ADS1299_FIELD_CONFIG3_BIAS_LOFF_SENS, 0u,
                     config->enable_bias_leadoff_sense ? 1u : 0u,
                     variant, &config3) != 0) {
        return ADS1299_EINVAL;
    }

    uint8_t previous_config3 = 0u;
    ads1299_status_t rc = ads1299_read_register(dev, ADS1299_REG_CONFIG3,
                                                &previous_config3);
    if (rc != ADS1299_OK) return rc;

    rc = ads1299_safe_write_field(dev, ADS1299_FIELD_BIAS_SENSP_MASK, 0u,
                                  positive_mask, variant, NULL);
    if (rc != ADS1299_OK) return rc;
    rc = ads1299_safe_write_field(dev, ADS1299_FIELD_BIAS_SENSN_MASK, 0u,
                                  negative_mask, variant, NULL);
    if (rc != ADS1299_OK) return rc;
    rc = ads1299_strict_write_register(dev, ADS1299_REG_CONFIG3, config3, variant);
    if (rc != ADS1299_OK) return rc;

    if (config->enable_reference_buffer &&
        !(previous_config3 & ADS1299_CONFIG3_REFBUF_ENABLE)) {
        dev->port.delay_us(dev->port.user, 150000u);
    }
    return ADS1299_OK;
}

ads1299_status_t ads1299_read_bias_status(ads1299_t *dev,
                                          uint8_t *bias_off) {
    if (!dev || !bias_off) return ADS1299_EINVAL;
    const ads1299_variant_t variant = ads1299_effective_variant(dev);
    return ads1299_read_field(dev, ADS1299_FIELD_CONFIG3_BIAS_STAT, 0u,
                              variant, bias_off);
}

ads1299_status_t ads1299_configure_lead_off(ads1299_t *dev,
                                            const ads1299_leadoff_config_t *config) {
    if (!dev || !config) return ADS1299_EINVAL;
    const ads1299_variant_t variant = ads1299_effective_variant(dev);
    if (!ads1299_field_code_valid(ADS1299_FIELD_LOFF_COMP_TH,
                                  config->comparator_threshold_code, variant) ||
        !ads1299_field_code_valid(ADS1299_FIELD_LOFF_CURRENT,
                                  config->current_code, variant) ||
        !ads1299_field_code_valid(ADS1299_FIELD_LOFF_FREQ,
                                  config->frequency_code, variant)) {
        return ADS1299_EINVAL;
    }

    uint8_t loff = ADS1299_RESET_LOFF;
    if (encode_field(ADS1299_FIELD_LOFF_COMP_TH, 0u,
                     config->comparator_threshold_code, variant, &loff) != 0 ||
        encode_field(ADS1299_FIELD_LOFF_CURRENT, 0u,
                     config->current_code, variant, &loff) != 0 ||
        encode_field(ADS1299_FIELD_LOFF_FREQ, 0u,
                     config->frequency_code, variant, &loff) != 0) {
        return ADS1299_EINVAL;
    }

    const uint8_t chmask = ads1299_variant_channel_mask(variant);
    ads1299_status_t rc = ads1299_strict_write_register(dev, ADS1299_REG_LOFF,
                                                        loff, variant);
    if (rc != ADS1299_OK) return rc;
    rc = ads1299_safe_write_field(dev, ADS1299_FIELD_LOFF_SENSP_MASK, 0u,
                                  (uint8_t)(config->positive_sense_mask & chmask),
                                  variant, NULL);
    if (rc != ADS1299_OK) return rc;
    rc = ads1299_safe_write_field(dev, ADS1299_FIELD_LOFF_SENSN_MASK, 0u,
                                  (uint8_t)(config->negative_sense_mask & chmask),
                                  variant, NULL);
    if (rc != ADS1299_OK) return rc;
    rc = ads1299_safe_write_field(dev, ADS1299_FIELD_LOFF_FLIP_MASK, 0u,
                                  (uint8_t)(config->current_flip_mask & chmask),
                                  variant, NULL);
    if (rc != ADS1299_OK) return rc;
    return ads1299_safe_write_field(dev,
                                    ADS1299_FIELD_CONFIG4_LOFF_COMP_ENABLE,
                                    0u, config->enable_comparators ? 1u : 0u,
                                    variant, NULL);
}

ads1299_status_t ads1299_read_lead_off_status(ads1299_t *dev,
                                              ads1299_leadoff_status_t *status) {
    if (!dev || !status) return ADS1299_EINVAL;
    const ads1299_variant_t variant = ads1299_effective_variant(dev);
    ads1299_status_t rc = ads1299_read_field(dev, ADS1299_FIELD_LOFF_STATP_MASK,
                                             0u, variant,
                                             &status->positive_off_mask);
    if (rc != ADS1299_OK) return rc;
    rc = ads1299_read_field(dev, ADS1299_FIELD_LOFF_STATN_MASK, 0u, variant,
                            &status->negative_off_mask);
    if (rc != ADS1299_OK) return rc;
    return ads1299_read_bias_status(dev, &status->bias_off);
}

ads1299_status_t ads1299_gpio_configure(ads1299_t *dev,
                                        uint8_t direction_mask,
                                        uint8_t output_value_mask) {
    if (!dev) return ADS1299_EINVAL;
    const ads1299_variant_t variant = ads1299_effective_variant(dev);
    uint8_t value = ADS1299_RESET_GPIO;
    if (encode_field(ADS1299_FIELD_GPIO_DATA, 0u,
                     (uint8_t)(output_value_mask & 0x0Fu), variant, &value) != 0 ||
        encode_field(ADS1299_FIELD_GPIO_DIRECTION, 0u,
                     (uint8_t)(direction_mask & 0x0Fu), variant, &value) != 0) {
        return ADS1299_EINVAL;
    }
    return ads1299_strict_write_register(dev, ADS1299_REG_GPIO, value, variant);
}

ads1299_status_t ads1299_gpio_write(ads1299_t *dev,
                                    uint8_t output_value_mask) {
    if (!dev) return ADS1299_EINVAL;
    const ads1299_variant_t variant = ads1299_effective_variant(dev);
    return ads1299_safe_write_field(dev, ADS1299_FIELD_GPIO_DATA, 0u,
                                    (uint8_t)(output_value_mask & 0x0Fu),
                                    variant, NULL);
}

ads1299_status_t ads1299_gpio_read(ads1299_t *dev,
                                   uint8_t *pin_state_mask) {
    if (!dev || !pin_state_mask) return ADS1299_EINVAL;
    const ads1299_variant_t variant = ads1299_effective_variant(dev);
    return ads1299_read_field(dev, ADS1299_FIELD_GPIO_DATA, 0u, variant,
                              pin_state_mask);
}

double ads1299_code_to_microvolts(int32_t code,
                                  double vref_volts,
                                  double gain) {
    return ads1299_code_to_volts(code, vref_volts, gain) * 1000000.0;
}

/* ===== conversion ===== */
#include "ads1299.h"

/*
 * SBAS499C Rev. C has two closely related numerical conventions in its data
 * format material. Equation 8 defines one quantizer LSB as +FS / 2^23, while
 * Table 9 and TI support guidance commonly normalize the maximum positive code
 * 0x7FFFFF to +FS using (2^23 - 1). These functions keep the conventions
 * explicit instead of silently treating them as interchangeable.
 */

double ads1299_lsb_volts_equation8(double vref_volts, double gain) {
    if (gain <= 0.0 || vref_volts <= 0.0) return 0.0;
    return vref_volts /
           (gain * (double)ADS1299_ADC_LSB_CODE_SCALE);
}

double ads1299_code_to_volts_equation8(int32_t code,
                                        double vref_volts,
                                        double gain) {
    const double lsb = ads1299_lsb_volts_equation8(vref_volts, gain);
    if (lsb == 0.0) return 0.0;
    return (double)code * lsb;
}

double ads1299_code_to_volts_positive_fs(int32_t code,
                                         double vref_volts,
                                         double gain) {
    return ads1299_code_to_volts(code, vref_volts, gain);
}

/* ===== diagnostics ===== */
#include "ads1299.h"

ads1299_status_t ads1299_temperature_c_from_microvolts(double sensor_uv,
                                                        double *temperature_c) {
    if (!temperature_c) return ADS1299_EINVAL;
    *temperature_c =
        ((sensor_uv - ADS1299_TEMP_SENSOR_25C_UV) /
         ADS1299_TEMP_SENSOR_UV_PER_C) + ADS1299_TEMP_SENSOR_REFERENCE_C;
    return ADS1299_OK;
}

ads1299_status_t ads1299_mvdd_mux_input_volts(uint8_t channel_1_to_8,
                                               double avdd_v,
                                               double avss_v,
                                               double dvdd_v,
                                               double *mux_input_v) {
    if (!mux_input_v || channel_1_to_8 < 1u || channel_1_to_8 > 8u) {
        return ADS1299_EINVAL;
    }

    if (channel_1_to_8 == 3u || channel_1_to_8 == 4u) {
        if (dvdd_v <= 0.0) return ADS1299_EINVAL;
        *mux_input_v = dvdd_v / 4.0;
        return ADS1299_OK;
    }

    if (avdd_v <= avss_v) return ADS1299_EINVAL;
    *mux_input_v = 0.5 * (avdd_v + avss_v);
    return ADS1299_OK;
}

ads1299_status_t ads1299_configure_temperature_measurement(
    ads1299_t *dev,
    uint8_t channel_1_to_8,
    uint8_t gain_code) {
    return ads1299_set_channel(dev, channel_1_to_8, gain_code,
                               ADS1299_MUX_TEMP, 0, 0);
}

ads1299_status_t ads1299_configure_supply_measurement(
    ads1299_t *dev,
    uint8_t channel_1_to_8) {
    return ads1299_set_channel(dev, channel_1_to_8, ADS1299_GAIN_1,
                               ADS1299_MUX_MVDD, 0, 0);
}

/* ===== runtime ===== */
#include "ads1299.h"

#define ADS1299_STANDBY_WAKEUP_US 32u
#define ADS1299_POWERUP_SETTLE_US 150000u
#define ADS1299_START_PULSE_US 2u

ads1299_status_t ads1299_wakeup(ads1299_t *dev) {
    if (!dev) return ADS1299_EINVAL;
    ads1299_status_t rc = ads1299_command(dev, ADS1299_CMD_WAKEUP);
    if (rc == ADS1299_OK) {
        /* SBAS499C specifies 31.25 us from WAKEUP to an active device. */
        dev->port.delay_us(dev->port.user, ADS1299_STANDBY_WAKEUP_US);
        dev->standby_mode = 0u;
    }
    return rc;
}

ads1299_status_t ads1299_standby(ads1299_t *dev) {
    if (!dev) return ADS1299_EINVAL;
    ads1299_status_t rc = ads1299_command(dev, ADS1299_CMD_STANDBY);
    if (rc == ADS1299_OK) {
        dev->standby_mode = 1u;
        dev->continuous_mode = 0u;
    }
    return rc;
}

ads1299_status_t ads1299_set_power_down(ads1299_t *dev, int power_down) {
    if (!dev || !dev->port.pwdn_write) return ADS1299_EINVAL;
    dev->port.pwdn_write(dev->port.user, power_down ? 0 : 1);
    if (power_down) {
        dev->continuous_mode = 0u;
        dev->standby_mode = 0u;
    } else {
        /* Conservative power-up guard covering oscillator/reference startup
         * and the datasheet's 150 ms internal-reference settling time. */
        dev->port.delay_us(dev->port.user, ADS1299_POWERUP_SETTLE_US);
    }
    return ADS1299_OK;
}

ads1299_status_t ads1299_start_pin(ads1299_t *dev) {
    if (!dev || !dev->port.start_write || !dev->port.delay_us) return ADS1299_EINVAL;
    if (dev->standby_mode) return ADS1299_ESTATE;
    dev->port.start_write(dev->port.user, 1);
    dev->port.delay_us(dev->port.user, ADS1299_START_PULSE_US);
    return ADS1299_OK;
}

ads1299_status_t ads1299_stop_pin(ads1299_t *dev) {
    if (!dev || !dev->port.start_write) return ADS1299_EINVAL;
    if (dev->standby_mode) return ADS1299_ESTATE;
    dev->port.start_write(dev->port.user, 0);
    return ADS1299_OK;
}

ads1299_status_t ads1299_wait_drdy(ads1299_t *dev,
                                   uint32_t timeout_us,
                                   uint32_t poll_interval_us) {
    if (!dev || !dev->port.drdy_read || !dev->port.delay_us || timeout_us == 0u) {
        return ADS1299_EINVAL;
    }
    if (dev->standby_mode) return ADS1299_ESTATE;
    if (poll_interval_us == 0u) poll_interval_us = 1u;

    uint32_t elapsed = 0u;
    while (elapsed < timeout_us) {
        if (dev->port.drdy_read(dev->port.user) == 0) return ADS1299_OK;
        uint32_t step = poll_interval_us;
        if (step > timeout_us - elapsed) step = timeout_us - elapsed;
        dev->port.delay_us(dev->port.user, step);
        elapsed += step;
    }
    return ADS1299_ETIMEOUT;
}

ads1299_variant_t ads1299_effective_variant(const ads1299_t *dev) {
    if (dev) {
        if (dev->channel_count == 4u) return ADS1299_VARIANT_4CH;
        if (dev->channel_count == 6u) return ADS1299_VARIANT_6CH;
        if (dev->channel_count == 8u) return ADS1299_VARIANT_8CH;
    }
    return ADS1299_VARIANT_8CH;
}

ads1299_status_t ads1299_read_field(ads1299_t *dev,
                                    ads1299_field_id_t field,
                                    uint8_t channel_1_to_8,
                                    ads1299_variant_t variant,
                                    uint8_t *code) {
    if (!dev || !code || ads1299_variant_channel_mask(variant) == 0u) {
        return ADS1299_EINVAL;
    }

    uint8_t address = 0u;
    if (ads1299_field_register_address(field, channel_1_to_8, variant, &address) != 0) {
        return ADS1299_EINVAL;
    }

    uint8_t register_value = 0u;
    ads1299_status_t rc = ads1299_read_register(dev, address, &register_value);
    if (rc != ADS1299_OK) return rc;

    uint8_t decoded = 0u;
    if (ads1299_field_decode(field, register_value, &decoded) != 0) {
        return ADS1299_EINVAL;
    }
    if (!ads1299_field_code_valid(field, decoded, variant)) {
        return ADS1299_EVERIFY;
    }
    *code = decoded;
    return ADS1299_OK;
}

ads1299_status_t ads1299_safe_write_registers(ads1299_t *dev,
                                              uint8_t address,
                                              const uint8_t *requested,
                                              size_t count,
                                              ads1299_variant_t variant,
                                              uint8_t *written_values) {
    if (!dev || !requested || count == 0u || address > ADS1299_REG_LAST ||
        count > (size_t)(ADS1299_REG_LAST - address + 1u) ||
        count > ADS1299_REGISTER_COUNT) {
        return ADS1299_EINVAL;
    }

    uint8_t sanitized[ADS1299_REGISTER_COUNT] = {0};
    for (size_t i = 0; i < count; ++i) {
        if (ads1299_sanitize_register_write((uint8_t)(address + i), requested[i],
                                            variant, &sanitized[i]) != 0) {
            return ADS1299_EINVAL;
        }
    }

    ads1299_status_t rc = ads1299_write_registers(dev, address, sanitized, count);
    if (rc == ADS1299_OK && written_values) {
        for (size_t i = 0; i < count; ++i) written_values[i] = sanitized[i];
    }
    return rc;
}

ads1299_status_t ads1299_safe_write_register(ads1299_t *dev,
                                             uint8_t address,
                                             uint8_t requested,
                                             ads1299_variant_t variant,
                                             uint8_t *written_value) {
    return ads1299_safe_write_registers(dev, address, &requested, 1u, variant,
                                        written_value);
}

ads1299_status_t ads1299_strict_write_registers(ads1299_t *dev,
                                                uint8_t address,
                                                const uint8_t *values,
                                                size_t count,
                                                ads1299_variant_t variant) {
    if (!dev || !values || count == 0u || address > ADS1299_REG_LAST ||
        count > (size_t)(ADS1299_REG_LAST - address + 1u) ||
        count > ADS1299_REGISTER_COUNT) {
        return ADS1299_EINVAL;
    }

    /* Validate the complete sequence before any bus activity. */
    for (size_t i = 0; i < count; ++i) {
        if (!ads1299_register_write_value_valid((uint8_t)(address + i), values[i],
                                                variant)) {
            return ADS1299_EINVAL;
        }
    }
    return ads1299_write_registers(dev, address, values, count);
}

ads1299_status_t ads1299_strict_write_register(ads1299_t *dev,
                                               uint8_t address,
                                               uint8_t value,
                                               ads1299_variant_t variant) {
    return ads1299_strict_write_registers(dev, address, &value, 1u, variant);
}

ads1299_status_t ads1299_safe_update_register_bits(ads1299_t *dev,
                                                   uint8_t address,
                                                   uint8_t mask,
                                                   uint8_t value,
                                                   ads1299_variant_t variant,
                                                   uint8_t *written_value) {
    if (!dev || mask == 0u) return ADS1299_EINVAL;
    const ads1299_register_info_t *info = ads1299_register_info(address);
    if (!info || info->read_only || !ads1299_register_available(address, variant) ||
        (mask & (uint8_t)~info->writable_mask) != 0u) {
        return ADS1299_EINVAL;
    }

    uint8_t current = 0u;
    ads1299_status_t rc = ads1299_read_register(dev, address, &current);
    if (rc != ADS1299_OK) return rc;

    const uint8_t requested = (uint8_t)((current & (uint8_t)~mask) | (value & mask));
    return ads1299_safe_write_register(dev, address, requested, variant, written_value);
}

ads1299_status_t ads1299_safe_write_field(ads1299_t *dev,
                                          ads1299_field_id_t field,
                                          uint8_t channel_1_to_8,
                                          uint8_t code,
                                          ads1299_variant_t variant,
                                          uint8_t *written_register_value) {
    if (!dev) return ADS1299_EINVAL;

    uint8_t address = 0u;
    if (ads1299_field_register_address(field, channel_1_to_8, variant, &address) != 0 ||
        !ads1299_field_code_valid(field, code, variant)) {
        return ADS1299_EINVAL;
    }
    const ads1299_field_info_t *info = ads1299_field_info(field);
    if (!info || !info->writable) return ADS1299_EINVAL;

    uint8_t current = 0u;
    ads1299_status_t rc = ads1299_read_register(dev, address, &current);
    if (rc != ADS1299_OK) return rc;

    uint8_t requested = 0u;
    if (ads1299_field_encode(field, channel_1_to_8, code, current, variant,
                             &requested) != 0) {
        return ADS1299_EINVAL;
    }
    return ads1299_safe_write_register(dev, address, requested, variant,
                                       written_register_value);
}
