#include "ads1299_cli.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void emit(ads1299_cli_t *cli, const char *text) {
    if (cli && cli->write && text) cli->write(cli->user, text);
}

static ads1299_status_t finish(ads1299_cli_t *cli, ads1299_status_t rc) {
    char msg[32];
    if (rc == ADS1299_OK) {
        emit(cli, "OK\n");
    } else {
        (void)snprintf(msg, sizeof(msg), "ERR %d\n", (int)rc);
        emit(cli, msg);
    }
    return rc;
}

static int parse_u32(const char *text, uint32_t *value) {
    if (!text || !value || *text == '\0') return 0;
    char *end = NULL;
    unsigned long parsed = strtoul(text, &end, 0);
    if (!end || *end != '\0' || parsed > 0xFFFFFFFFul) return 0;
    *value = (uint32_t)parsed;
    return 1;
}

static int parse_on_off(const char *text, int *value) {
    if (!text || !value) return 0;
    if (strcmp(text, "on") == 0 || strcmp(text, "1") == 0) {
        *value = 1;
        return 1;
    }
    if (strcmp(text, "off") == 0 || strcmp(text, "0") == 0) {
        *value = 0;
        return 1;
    }
    return 0;
}

static int parse_input(const char *text, ads1299_control_input_t *input) {
    if (!text || !input) return 0;
    if (strcmp(text, "normal") == 0) *input = ADS1299_CONTROL_INPUT_NORMAL;
    else if (strcmp(text, "short") == 0) *input = ADS1299_CONTROL_INPUT_SHORTED;
    else if (strcmp(text, "bias") == 0) *input = ADS1299_CONTROL_INPUT_BIAS_MEAS;
    else if (strcmp(text, "mvdd") == 0) *input = ADS1299_CONTROL_INPUT_MVDD;
    else if (strcmp(text, "temp") == 0) *input = ADS1299_CONTROL_INPUT_TEMP;
    else if (strcmp(text, "test") == 0) *input = ADS1299_CONTROL_INPUT_TEST;
    else if (strcmp(text, "biasp") == 0) *input = ADS1299_CONTROL_INPUT_BIAS_DRP;
    else if (strcmp(text, "biasn") == 0) *input = ADS1299_CONTROL_INPUT_BIAS_DRN;
    else return 0;
    return 1;
}

static ads1299_status_t command_id(ads1299_cli_t *cli) {
    ads1299_device_id_t id;
    ads1299_status_t rc = ads1299_read_device_id(cli->control->dev, &id);
    if (rc != ADS1299_OK) return finish(cli, rc);

    char msg[96];
    (void)snprintf(msg, sizeof(msg),
                   "ID raw=0x%02X family=%u channels=%u revision=%u\n",
                   id.raw, (unsigned)id.is_ads1299_family,
                   (unsigned)id.channel_count, (unsigned)id.revision);
    emit(cli, msg);
    return finish(cli, ADS1299_OK);
}

static ads1299_status_t command_regdump(ads1299_cli_t *cli) {
    ads1299_register_dump_t dump;
    ads1299_status_t rc = ads1299_read_register_dump(cli->control->dev, &dump);
    if (rc != ADS1299_OK) return finish(cli, rc);

    char msg[32];
    for (uint8_t reg = 0u; reg < ADS1299_REGISTER_COUNT; ++reg) {
        (void)snprintf(msg, sizeof(msg), "R%02X=0x%02X\n",
                       reg, dump.value[reg]);
        emit(cli, msg);
    }
    return finish(cli, ADS1299_OK);
}

void ads1299_cli_print_help(ads1299_cli_t *cli) {
    emit(cli,
        "ADS1299 commands:\n"
        "  help\n"
        "  id | regdump\n"
        "  rate <250|500|1000|2000|4000|8000|16000>\n"
        "  gain <ch> <1|2|4|6|8|12|24>\n"
        "  input <ch> <normal|short|bias|mvdd|temp|test|biasp|biasn>\n"
        "  channel <ch> <on|off>\n"
        "  srb1 <on|off>\n"
        "  srb2 <ch> <on|off>\n"
        "  bias <on|off>\n"
        "  biasref <internal|external>\n"
        "  biasp <mask> | biasn <mask>   (mask accepts 0xNN)\n"
        "  test <1x|2x> <slow|fast|dc>\n"
        "  profile <eeg|short|test>\n"
        "  start | stop\n");
}

ads1299_status_t ads1299_cli_init(ads1299_cli_t *cli,
                                  ads1299_control_t *control,
                                  ads1299_cli_write_fn write,
                                  void *user) {
    if (!cli || !control || !control->dev || !write) return ADS1299_EINVAL;
    cli->control = control;
    cli->write = write;
    cli->user = user;
    return ADS1299_OK;
}

ads1299_status_t ads1299_cli_execute(ads1299_cli_t *cli, const char *line) {
    if (!cli || !cli->control || !cli->control->dev || !cli->write || !line) {
        return ADS1299_EINVAL;
    }

    const size_t len = strlen(line);
    if (len == 0u || len >= ADS1299_CLI_LINE_MAX) {
        return finish(cli, ADS1299_EINVAL);
    }

    char buffer[ADS1299_CLI_LINE_MAX];
    memcpy(buffer, line, len + 1u);

    char *cmd = strtok(buffer, " \t\r\n");
    if (!cmd) return finish(cli, ADS1299_EINVAL);

    if (strcmp(cmd, "help") == 0) {
        ads1299_cli_print_help(cli);
        return ADS1299_OK;
    }
    if (strcmp(cmd, "id") == 0) return command_id(cli);
    if (strcmp(cmd, "regdump") == 0) return command_regdump(cli);

    if (strcmp(cmd, "rate") == 0) {
        uint32_t rate = 0u;
        char *rate_text = strtok(NULL, " \t\r\n");
        if (!parse_u32(rate_text, &rate)) return finish(cli, ADS1299_EINVAL);
        return finish(cli, ads1299_control_set_rate_sps(cli->control, rate));
    }

    if (strcmp(cmd, "gain") == 0) {
        uint32_t channel = 0u;
        uint32_t gain = 0u;
        if (!parse_u32(strtok(NULL, " \t\r\n"), &channel) ||
            !parse_u32(strtok(NULL, " \t\r\n"), &gain) || channel > 255u) {
            return finish(cli, ADS1299_EINVAL);
        }
        return finish(cli, ads1299_control_set_gain(
                               cli->control, (uint8_t)channel, (unsigned)gain));
    }

    if (strcmp(cmd, "input") == 0) {
        uint32_t channel = 0u;
        ads1299_control_input_t input;
        if (!parse_u32(strtok(NULL, " \t\r\n"), &channel) ||
            channel > 255u ||
            !parse_input(strtok(NULL, " \t\r\n"), &input)) {
            return finish(cli, ADS1299_EINVAL);
        }
        return finish(cli, ads1299_control_set_input(
                               cli->control, (uint8_t)channel, input));
    }

    if (strcmp(cmd, "channel") == 0) {
        uint32_t channel = 0u;
        int enable = 0;
        if (!parse_u32(strtok(NULL, " \t\r\n"), &channel) ||
            channel > 255u ||
            !parse_on_off(strtok(NULL, " \t\r\n"), &enable)) {
            return finish(cli, ADS1299_EINVAL);
        }
        return finish(cli, ads1299_control_set_channel_enabled(
                               cli->control, (uint8_t)channel, enable));
    }

    if (strcmp(cmd, "srb1") == 0) {
        int enable = 0;
        if (!parse_on_off(strtok(NULL, " \t\r\n"), &enable)) {
            return finish(cli, ADS1299_EINVAL);
        }
        return finish(cli, ads1299_control_set_srb1(cli->control, enable));
    }

    if (strcmp(cmd, "srb2") == 0) {
        uint32_t channel = 0u;
        int enable = 0;
        if (!parse_u32(strtok(NULL, " \t\r\n"), &channel) ||
            channel > 255u ||
            !parse_on_off(strtok(NULL, " \t\r\n"), &enable)) {
            return finish(cli, ADS1299_EINVAL);
        }
        return finish(cli, ads1299_control_set_srb2(
                               cli->control, (uint8_t)channel, enable));
    }

    if (strcmp(cmd, "bias") == 0) {
        int enable = 0;
        if (!parse_on_off(strtok(NULL, " \t\r\n"), &enable)) {
            return finish(cli, ADS1299_EINVAL);
        }
        return finish(cli,
                      ads1299_control_set_bias_enabled(cli->control, enable));
    }

    if (strcmp(cmd, "biasref") == 0) {
        char *mode = strtok(NULL, " \t\r\n");
        if (!mode) return finish(cli, ADS1299_EINVAL);
        if (strcmp(mode, "internal") == 0) {
            return finish(cli, ads1299_control_set_bias_reference_internal(
                                   cli->control, 1));
        }
        if (strcmp(mode, "external") == 0) {
            return finish(cli, ads1299_control_set_bias_reference_internal(
                                   cli->control, 0));
        }
        return finish(cli, ADS1299_EINVAL);
    }

    if (strcmp(cmd, "biasp") == 0 || strcmp(cmd, "biasn") == 0) {
        uint32_t mask = 0u;
        if (!parse_u32(strtok(NULL, " \t\r\n"), &mask) || mask > 0xFFu) {
            return finish(cli, ADS1299_EINVAL);
        }
        ads1299_bias_config_t bias;
        ads1299_status_t rc = ads1299_control_read_bias_config(cli->control,
                                                               &bias);
        if (rc != ADS1299_OK) return finish(cli, rc);
        if (strcmp(cmd, "biasp") == 0) bias.positive_sense_mask = (uint8_t)mask;
        else bias.negative_sense_mask = (uint8_t)mask;
        return finish(cli, ads1299_configure_bias(cli->control->dev, &bias));
    }

    if (strcmp(cmd, "test") == 0) {
        char *amp = strtok(NULL, " \t\r\n");
        char *freq = strtok(NULL, " \t\r\n");
        if (!amp || !freq) return finish(cli, ADS1299_EINVAL);

        int amplitude_x2;
        if (strcmp(amp, "1x") == 0) amplitude_x2 = 0;
        else if (strcmp(amp, "2x") == 0) amplitude_x2 = 1;
        else return finish(cli, ADS1299_EINVAL);

        uint8_t freq_code;
        if (strcmp(freq, "slow") == 0) freq_code = ADS1299_TEST_FREQ_FCLK_2_21;
        else if (strcmp(freq, "fast") == 0) freq_code = ADS1299_TEST_FREQ_FCLK_2_20;
        else if (strcmp(freq, "dc") == 0) freq_code = ADS1299_TEST_FREQ_DC;
        else return finish(cli, ADS1299_EINVAL);

        return finish(cli, ads1299_control_configure_test(
                               cli->control, amplitude_x2, freq_code));
    }

    if (strcmp(cmd, "profile") == 0) {
        char *name = strtok(NULL, " \t\r\n");
        if (!name) return finish(cli, ADS1299_EINVAL);
        ads1299_control_profile_t profile;
        if (strcmp(name, "eeg") == 0) profile = ADS1299_CONTROL_PROFILE_EEG;
        else if (strcmp(name, "short") == 0) {
            profile = ADS1299_CONTROL_PROFILE_INPUT_SHORT;
        } else if (strcmp(name, "test") == 0) {
            profile = ADS1299_CONTROL_PROFILE_INTERNAL_TEST;
        } else {
            return finish(cli, ADS1299_EINVAL);
        }
        return finish(cli, ads1299_control_apply_profile(cli->control,
                                                         profile, NULL));
    }

    if (strcmp(cmd, "start") == 0) {
        return finish(cli, ads1299_control_start_stream(cli->control));
    }
    if (strcmp(cmd, "stop") == 0) {
        return finish(cli, ads1299_control_stop_stream(cli->control));
    }

    return finish(cli, ADS1299_EINVAL);
}
