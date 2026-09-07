#include "ads1299.h"

#include <string.h>

#include "ads1299.h"
#include "ads1299.h"

static int valid_group_command(uint8_t command) {
    switch (command) {
        case ADS1299_CMD_WAKEUP:
        case ADS1299_CMD_STANDBY:
        case ADS1299_CMD_RESET:
        case ADS1299_CMD_START:
        case ADS1299_CMD_STOP:
        case ADS1299_CMD_RDATAC:
        case ADS1299_CMD_SDATAC:
            return 1;
        default:
            return 0;
    }
}

ads1299_status_t ads1299_read_configuration(ads1299_t *dev,
                                            ads1299_configuration_t *configuration) {
    if (!dev || !configuration) return ADS1299_EINVAL;
    memset(configuration, 0, sizeof(*configuration));
    const ads1299_variant_t variant = ads1299_effective_variant(dev);
    const size_t first_block_count = (size_t)ADS1299_REG_CH1SET + (size_t)variant;
    ads1299_status_t rc = ads1299_read_registers(
        dev, ADS1299_REG_ID, configuration->value, first_block_count);
    if (rc != ADS1299_OK) return rc;
    return ads1299_read_registers(
        dev, ADS1299_REG_BIAS_SENSP,
        &configuration->value[ADS1299_REG_BIAS_SENSP],
        ADS1299_REGISTER_COUNT - ADS1299_REG_BIAS_SENSP);
}

static ads1299_status_t apply_range(ads1299_t *dev,
                                    const ads1299_configuration_t *configuration,
                                    ads1299_variant_t variant,
                                    uint8_t first,
                                    size_t count,
                                    int verify_after_write) {
    uint8_t written[ADS1299_REGISTER_COUNT] = {0};
    ads1299_status_t rc = ads1299_safe_write_registers(
        dev, first, &configuration->value[first], count, variant, written);
    if (rc != ADS1299_OK || !verify_after_write) return rc;

    for (size_t i = 0u; i < count; ++i) {
        const ads1299_register_info_t *info = ads1299_register_info((uint8_t)(first + i));
        uint8_t actual = 0u;
        rc = ads1299_verify_register(dev, (uint8_t)(first + i), written[i],
                                     info ? info->writable_mask : 0u, &actual);
        if (rc != ADS1299_OK) return rc;
    }
    return ADS1299_OK;
}

ads1299_status_t ads1299_apply_configuration(ads1299_t *dev,
                                             const ads1299_configuration_t *configuration,
                                             int verify_after_write) {
    if (!dev || !configuration) return ADS1299_EINVAL;
    const ads1299_variant_t variant = ads1299_effective_variant(dev);
    const size_t channel_count = (size_t)variant;
    ads1299_status_t rc = apply_range(dev, configuration, variant,
                                      ADS1299_REG_CONFIG1, 4u, verify_after_write);
    if (rc != ADS1299_OK) return rc;
    rc = apply_range(dev, configuration, variant, ADS1299_REG_CH1SET,
                     channel_count, verify_after_write);
    if (rc != ADS1299_OK) return rc;
    rc = apply_range(dev, configuration, variant, ADS1299_REG_BIAS_SENSP,
                     5u, verify_after_write);
    if (rc != ADS1299_OK) return rc;
    rc = apply_range(dev, configuration, variant, ADS1299_REG_GPIO,
                     2u, verify_after_write);
    if (rc != ADS1299_OK) return rc;
    return apply_range(dev, configuration, variant, ADS1299_REG_CONFIG4,
                       1u, verify_after_write);
}

ads1299_status_t ads1299_group_init(ads1299_group_t *group,
                                    ads1299_t **devices,
                                    size_t count) {
    if (!group || !devices || count == 0u || count > ADS1299_MAX_CHAIN_DEVICES) {
        return ADS1299_EINVAL;
    }
    for (size_t i = 0u; i < count; ++i) {
        if (!devices[i]) return ADS1299_EINVAL;
    }
    group->devices = devices;
    group->count = count;
    return ADS1299_OK;
}

ads1299_status_t ads1299_group_command(ads1299_group_t *group, uint8_t command) {
    if (!group || !group->devices || group->count == 0u ||
        group->count > ADS1299_MAX_CHAIN_DEVICES || !valid_group_command(command)) {
        return ADS1299_EINVAL;
    }
    for (size_t i = 0u; i < group->count; ++i) {
        ads1299_status_t rc;
        switch (command) {
            case ADS1299_CMD_WAKEUP: rc = ads1299_wakeup(group->devices[i]); break;
            case ADS1299_CMD_STANDBY: rc = ads1299_standby(group->devices[i]); break;
            case ADS1299_CMD_RESET: rc = ads1299_reset_command(group->devices[i]); break;
            case ADS1299_CMD_START: rc = ads1299_start(group->devices[i]); break;
            case ADS1299_CMD_STOP: rc = ads1299_stop(group->devices[i]); break;
            case ADS1299_CMD_RDATAC: rc = ads1299_rdatac(group->devices[i]); break;
            case ADS1299_CMD_SDATAC: rc = ads1299_sdatac(group->devices[i]); break;
            default: return ADS1299_EINVAL;
        }
        if (rc != ADS1299_OK) return rc;
    }
    return ADS1299_OK;
}

ads1299_status_t ads1299_group_apply_configuration(
    ads1299_group_t *group,
    const ads1299_configuration_t *configuration,
    int verify_after_write) {
    if (!group || !group->devices || !configuration || group->count == 0u ||
        group->count > ADS1299_MAX_CHAIN_DEVICES) return ADS1299_EINVAL;
    for (size_t i = 0u; i < group->count; ++i) {
        ads1299_status_t rc = ads1299_apply_configuration(
            group->devices[i], configuration, verify_after_write);
        if (rc != ADS1299_OK) return rc;
    }
    return ADS1299_OK;
}

size_t ads1299_daisy_frame_bytes(const ads1299_variant_t *variants, size_t count) {
    if (!variants || count == 0u || count > ADS1299_MAX_CHAIN_DEVICES) return 0u;
    size_t total = 0u;
    for (size_t i = 0u; i < count; ++i) {
        const size_t bytes = ads1299_frame_bytes_for_variant(variants[i]);
        if (bytes == 0u) return 0u;
        total += bytes;
    }
    return total;
}

ads1299_status_t ads1299_decode_daisy_frame(
    const uint8_t *raw,
    size_t raw_len,
    const ads1299_variant_t *variants,
    size_t count,
    ads1299_chain_frame_t *frames) {
    if (!raw || !variants || !frames ||
        raw_len != ads1299_daisy_frame_bytes(variants, count)) {
        return ADS1299_EINVAL;
    }
    size_t offset = 0u;
    for (size_t i = 0u; i < count; ++i) {
        const size_t bytes = ads1299_frame_bytes_for_variant(variants[i]);
        frames[i].variant = variants[i];
        ads1299_status_t rc = ads1299_decode_frame_variant(
            &raw[offset], bytes, variants[i], &frames[i].frame, NULL);
        if (rc != ADS1299_OK) return rc;
        offset += bytes;
    }
    return ADS1299_OK;
}

ads1299_status_t ads1299_read_daisy_frame_continuous(
    ads1299_t *head,
    const ads1299_variant_t *variants,
    size_t count,
    ads1299_chain_frame_t *frames) {
    if (!head || !head->continuous_mode || !head->port.spi_transfer ||
        !head->port.cs_write || !variants || !frames) return ADS1299_ESTATE;
    const size_t bytes = ads1299_daisy_frame_bytes(variants, count);
    if (bytes == 0u) return ADS1299_EINVAL;
    uint8_t tx[ADS1299_MAX_CHAIN_DEVICES * ADS1299_FRAME_BYTES] = {0};
    uint8_t rx[ADS1299_MAX_CHAIN_DEVICES * ADS1299_FRAME_BYTES] = {0};
    head->port.cs_write(head->port.user, 0);
    const int io_rc = head->port.spi_transfer(head->port.user, tx, rx, bytes);
    head->port.cs_write(head->port.user, 1);
    if (io_rc != 0) return ADS1299_EIO;
    return ads1299_decode_daisy_frame(rx, bytes, variants, count, frames);
}
