#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "ads1299.h"

typedef struct {
    size_t spi_calls;
    size_t spi_len[24];
    uint8_t spi_first_tx[24];
    size_t delay_calls;
    uint32_t delays_us[24];
    size_t cs_calls;
    int cs_levels[24];
    int start_level;
    int reset_level;
    uint8_t read_fill;
} mock_port_t;

static int mock_spi_transfer(void *user, const uint8_t *tx, uint8_t *rx, size_t len) {
    mock_port_t *mock = (mock_port_t *)user;
    assert(mock->spi_calls < 24u);
    mock->spi_len[mock->spi_calls] = len;
    mock->spi_first_tx[mock->spi_calls] = (tx && len > 0u) ? tx[0] : 0u;
    mock->spi_calls++;
    if (rx) for (size_t i = 0; i < len; ++i) rx[i] = mock->read_fill;
    return 0;
}
static void mock_cs_write(void *user, int level) {
    mock_port_t *mock = (mock_port_t *)user;
    assert(mock->cs_calls < 24u);
    mock->cs_levels[mock->cs_calls++] = level;
}
static void mock_start_write(void *user, int level) { ((mock_port_t *)user)->start_level = level; }
static void mock_reset_write(void *user, int level) { ((mock_port_t *)user)->reset_level = level; }
static void mock_delay_us(void *user, uint32_t us) {
    mock_port_t *mock = (mock_port_t *)user;
    assert(mock->delay_calls < 24u);
    mock->delays_us[mock->delay_calls++] = us;
}
static void clear_io_log(mock_port_t *mock) {
    mock->spi_calls = mock->delay_calls = mock->cs_calls = 0u;
    memset(mock->spi_len, 0, sizeof(mock->spi_len));
    memset(mock->spi_first_tx, 0, sizeof(mock->spi_first_tx));
    memset(mock->delays_us, 0, sizeof(mock->delays_us));
    memset(mock->cs_levels, 0, sizeof(mock->cs_levels));
}
static ads1299_port_t make_port(mock_port_t *mock) {
    ads1299_port_t port = {.user=mock,.spi_transfer=mock_spi_transfer,.cs_write=mock_cs_write,
        .reset_write=mock_reset_write,.start_write=mock_start_write,.drdy_read=NULL,.delay_us=mock_delay_us};
    return port;
}

static void test_math_and_config(void) {
    assert(ads1299_sign_extend24(0x000000u)==0);
    assert(ads1299_sign_extend24(0x7FFFFFu)==8388607);
    assert(ads1299_sign_extend24(0x800000u)==-8388608);
    assert(ads1299_sign_extend24(0xFFFFFFu)==-1);
    double v=ads1299_code_to_volts(8388607,4.5,24.0); assert(v>0.18749&&v<0.18751);
    assert(ads1299_make_internal_test_config2(0,ADS1299_TEST_FREQ_FCLK_2_21)==0xD0u);
    assert(ads1299_make_internal_test_config2(1,ADS1299_TEST_FREQ_FCLK_2_20)==0xD5u);
    assert(ads1299_make_internal_test_config2(0,ADS1299_TEST_FREQ_DC)==0xD3u);
    const ads1299_test_signal_config_t ext = {
        .use_internal_source = 0u,
        .amplitude_x2 = 1u,
        .frequency_code = ADS1299_TEST_FREQ_DC,
    };
    assert(ads1299_make_test_config2(&ext) == 0xC7u);
    uint8_t checked = 0u;
    assert(ads1299_build_test_config2(&ext, &checked) == ADS1299_OK);
    assert(checked == 0xC7u);
    const ads1299_test_signal_config_t in = {
        .use_internal_source = 1u,
        .amplitude_x2 = 0u,
        .frequency_code = ADS1299_TEST_FREQ_FCLK_2_20,
    };
    assert(ads1299_make_test_config2(&in) == 0xD1u);
    assert(ads1299_build_test_config2(&in, &checked) == ADS1299_OK);
    assert(checked == 0xD1u);

    const ads1299_test_signal_config_t invalid_frequency = {
        .use_internal_source = 1u,
        .amplitude_x2 = 0u,
        .frequency_code = ADS1299_TEST_FREQ_RESERVED,
    };
    assert(ads1299_build_test_config2(&invalid_frequency, &checked) == ADS1299_EINVAL);
    assert(ads1299_make_test_config2(&invalid_frequency) == ADS1299_CONFIG2_RESERVED_BASE);
    const ads1299_test_signal_config_t invalid_boolean = {
        .use_internal_source = 2u,
        .amplitude_x2 = 0u,
        .frequency_code = ADS1299_TEST_FREQ_DC,
    };
    assert(ads1299_build_test_config2(&invalid_boolean, &checked) == ADS1299_EINVAL);
}

static void test_init_contract(void) {
    mock_port_t mock={0}; ads1299_port_t port=make_port(&mock); ads1299_t dev;
    assert(ads1299_init(&dev,&port)==ADS1299_OK);
    assert(mock.cs_calls==1u&&mock.cs_levels[0]==1); assert(mock.start_level==0); assert(mock.reset_level==1);
    assert(dev.continuous_mode==0u && dev.standby_mode==0u && dev.channel_count==0u);
}

static void test_wreg_always_establishes_command_mode(void) {
    mock_port_t mock={0}; ads1299_port_t port=make_port(&mock); ads1299_t dev; const uint8_t value=0x96u;
    assert(ads1299_init(&dev,&port)==ADS1299_OK); clear_io_log(&mock);
    assert(ads1299_write_register(&dev,ADS1299_REG_CONFIG1,value)==ADS1299_OK);
    assert(mock.spi_calls==4u);
    assert(mock.spi_first_tx[0]==ADS1299_CMD_SDATAC);
    assert(mock.spi_first_tx[1]==(uint8_t)(ADS1299_CMD_WREG|ADS1299_REG_CONFIG1));
    assert(mock.spi_first_tx[2]==0u); assert(mock.spi_first_tx[3]==value);
    assert(dev.continuous_mode==0u);
}

static void test_rreg_always_establishes_command_mode(void) {
    mock_port_t mock={0}; ads1299_port_t port=make_port(&mock); ads1299_t dev; uint8_t value=0u;
    mock.read_fill=0xA5u; assert(ads1299_init(&dev,&port)==ADS1299_OK); clear_io_log(&mock);
    assert(ads1299_read_register(&dev,ADS1299_REG_CONFIG1,&value)==ADS1299_OK); assert(value==0xA5u);
    assert(mock.spi_calls==4u); assert(mock.spi_first_tx[0]==ADS1299_CMD_SDATAC);
    assert(mock.spi_first_tx[1]==(uint8_t)(ADS1299_CMD_RREG|ADS1299_REG_CONFIG1));
    assert(dev.continuous_mode==0u);
}

static void test_stale_stream_flag_cannot_break_register_access(void) {
    mock_port_t mock={0}; ads1299_port_t port=make_port(&mock); ads1299_t dev; uint8_t value=0u;
    assert(ads1299_init(&dev,&port)==ADS1299_OK); dev.continuous_mode=0u; clear_io_log(&mock);
    assert(ads1299_read_register(&dev,ADS1299_REG_CONFIG1,&value)==ADS1299_OK);
    assert(mock.spi_first_tx[0]==ADS1299_CMD_SDATAC);
    dev.continuous_mode=1u; clear_io_log(&mock);
    assert(ads1299_read_register(&dev,ADS1299_REG_CONFIG1,&value)==ADS1299_OK);
    assert(mock.spi_first_tx[0]==ADS1299_CMD_SDATAC);
}

static void test_rdata_always_establishes_command_mode(void) {
    mock_port_t mock={0}; ads1299_port_t port=make_port(&mock); ads1299_t dev; ads1299_frame_t frame;
    mock.read_fill=0xC0u; assert(ads1299_init(&dev,&port)==ADS1299_OK); clear_io_log(&mock);
    assert(ads1299_read_frame_rdata(&dev,&frame)==ADS1299_OK);
    assert(mock.spi_first_tx[0]==ADS1299_CMD_SDATAC); assert(mock.spi_first_tx[1]==ADS1299_CMD_RDATA);
}

static void test_invalid_register_write_is_side_effect_free(void) {
    mock_port_t mock={0}; ads1299_port_t port=make_port(&mock); ads1299_t dev;
    assert(ads1299_init(&dev,&port)==ADS1299_OK); clear_io_log(&mock);
    assert(ads1299_write_register(&dev,ADS1299_REG_ID,0u)==ADS1299_EINVAL);
    assert(mock.spi_calls==0u&&mock.cs_calls==0u);
}

int main(void) {
    test_math_and_config(); test_init_contract(); test_wreg_always_establishes_command_mode();
    test_rreg_always_establishes_command_mode(); test_stale_stream_flag_cannot_break_register_access();
    test_rdata_always_establishes_command_mode(); test_invalid_register_write_is_side_effect_free();
    puts("ADS1299 deterministic command-mode and checked CONFIG2 tests passed"); return 0;
}
