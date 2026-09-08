#include "stm32c031_beginner_demo.h"

#include <stddef.h>
#include <stdint.h>

#include "ads1299.h"
#include "ads1299_packet.h"
#include "ads1299_spi.h"
#include "stm32c031_example_platform.h"

#define DEMO_DRDY_TIMEOUT_US 1000000u
#define DEMO_DRDY_POLL_US       1000u
#define DEMO_TEST_FRAMES            8u

static int demo_wait_and_read(ads1299_t *dev, ads1299_frame_t *frame)
{
    ads1299_status_t st = ads1299_wait_drdy(dev,
                                            DEMO_DRDY_TIMEOUT_US,
                                            DEMO_DRDY_POLL_US);
    if (st != ADS1299_OK) {
        stm32c031_example_log("ERR DRDY timeout/check DRDY wiring\r\n");
        return -1;
    }
    st = ads1299_read_frame_continuous(dev, frame);
    if (st != ADS1299_OK) {
        stm32c031_example_log("ERR frame read/check SPI and CS\r\n");
        return -2;
    }
    return 0;
}

static int demo_capture_frames(ads1299_t *dev, uint32_t count)
{
    ads1299_frame_t frame;
    uint32_t i;

    for (i = 0u; i < count; ++i) {
        if (demo_wait_and_read(dev, &frame) != 0) {
            return -1;
        }
    }
    return 0;
}

static int demo_prepare_continuous(ads1299_t *dev)
{
    if (ads1299_rdatac(dev) != ADS1299_OK) {
        stm32c031_example_log("ERR RDATAC\r\n");
        return -1;
    }
    if (ads1299_start(dev) != ADS1299_OK) {
        stm32c031_example_log("ERR START\r\n");
        return -2;
    }
    return 0;
}

static int demo_stop_continuous(ads1299_t *dev)
{
    ads1299_status_t a = ads1299_stop(dev);
    ads1299_status_t b = ads1299_sdatac(dev);
    return (a == ADS1299_OK && b == ADS1299_OK) ? 0 : -1;
}

static int demo_probe(ads1299_t *dev)
{
    ads1299_device_id_t id;

    if (ads1299_hardware_reset(dev) != ADS1299_OK) {
        stm32c031_example_log("ERR RESET/check RESET PWDN power\r\n");
        return -1;
    }
    if (ads1299_sdatac(dev) != ADS1299_OK) {
        stm32c031_example_log("ERR SDATAC/check SPI Mode 1 CS\r\n");
        return -2;
    }
    if (ads1299_read_device_id(dev, &id) != ADS1299_OK ||
        !id.is_ads1299_family) {
        stm32c031_example_log("ERR ID/not ADS1299 family; check SPI Mode 1 power clock\r\n");
        return -3;
    }

    if (id.channel_count == 8u) {
        stm32c031_example_log("OK ID ADS1299-8\r\n");
    } else if (id.channel_count == 6u) {
        stm32c031_example_log("OK ID ADS1299-6\r\n");
    } else if (id.channel_count == 4u) {
        stm32c031_example_log("OK ID ADS1299-4\r\n");
    } else {
        stm32c031_example_log("ERR ID channel variant unexpected\r\n");
        return -4;
    }
    return 0;
}

static int demo_internal_test(ads1299_t *dev)
{
    stm32c031_example_log("RUN internal-test\r\n");
    if (ads1299_sdatac(dev) != ADS1299_OK) {
        return -1;
    }
    if (ads1299_configure_internal_test(dev,
                                        ADS1299_GAIN_24,
                                        0,
                                        ADS1299_TEST_FREQ_FCLK_DIV_2_21) != ADS1299_OK) {
        stm32c031_example_log("ERR internal-test config\r\n");
        return -2;
    }
    if (demo_prepare_continuous(dev) != 0) {
        return -3;
    }
    if (demo_capture_frames(dev, DEMO_TEST_FRAMES) != 0) {
        (void)demo_stop_continuous(dev);
        return -4;
    }
    if (demo_stop_continuous(dev) != 0) {
        return -5;
    }
    stm32c031_example_log("OK internal-test frames\r\n");
    return 0;
}

static int demo_input_short(ads1299_t *dev)
{
    stm32c031_example_log("RUN input-short\r\n");
    if (ads1299_sdatac(dev) != ADS1299_OK) {
        return -1;
    }
    if (ads1299_configure_input_short_test(dev, ADS1299_GAIN_24) != ADS1299_OK) {
        stm32c031_example_log("ERR input-short config\r\n");
        return -2;
    }
    if (demo_prepare_continuous(dev) != 0) {
        return -3;
    }
    if (demo_capture_frames(dev, DEMO_TEST_FRAMES) != 0) {
        (void)demo_stop_continuous(dev);
        return -4;
    }
    if (demo_stop_continuous(dev) != 0) {
        return -5;
    }
    stm32c031_example_log("OK input-short frames\r\n");
    return 0;
}

static int demo_configure_eeg_250sps(ads1299_t *dev)
{
    uint8_t ch;
    uint8_t channel_count = ads1299_effective_variant(dev).channel_count;

    stm32c031_example_log("RUN EEG 250 SPS config\r\n");
    if (ads1299_sdatac(dev) != ADS1299_OK) {
        return -1;
    }
    if (ads1299_set_data_rate(dev, ADS1299_DR_250SPS) != ADS1299_OK) {
        stm32c031_example_log("ERR 250-SPS data-rate config\r\n");
        return -2;
    }

    if (channel_count == 0u || channel_count > ADS1299_CHANNEL_COUNT) {
        channel_count = ADS1299_CHANNEL_COUNT;
    }
    for (ch = 1u; ch <= channel_count; ++ch) {
        if (ads1299_set_channel(dev,
                                ch,
                                ADS1299_GAIN_24,
                                ADS1299_MUX_NORMAL,
                                0,
                                0) != ADS1299_OK) {
            stm32c031_example_log("ERR EEG channel config\r\n");
            return -3;
        }
    }
    stm32c031_example_log("OK EEG 250 SPS configured\r\n");
    return 0;
}

static int demo_stream(ads1299_t *dev, uint32_t stream_frames)
{
    ads1299_frame_t frame;
    uint8_t packet[ADS1299_PACKET_SIZE];
    uint32_t sequence = 0u;

    stm32c031_example_log("RUN stream canonical 49-byte packets\r\n");
    if (demo_prepare_continuous(dev) != 0) {
        return -1;
    }

    while (stream_frames == 0u || sequence < stream_frames) {
        size_t packet_len;
        if (demo_wait_and_read(dev, &frame) != 0) {
            (void)demo_stop_continuous(dev);
            return -2;
        }

        packet_len = ads1299_packet_encode(packet,
                                           sizeof(packet),
                                           0u,
                                           sequence,
                                           stm32c031_example_timestamp_us(),
                                           frame.status,
                                           frame.channel);
        if (packet_len != ADS1299_PACKET_SIZE) {
            stm32c031_example_log("ERR packet encode\r\n");
            (void)demo_stop_continuous(dev);
            return -3;
        }
        if (stm32c031_example_write(packet, packet_len) != 0) {
            stm32c031_example_log("ERR stream transport/check UART config\r\n");
            (void)demo_stop_continuous(dev);
            return -4;
        }
        ++sequence;
    }

    if (demo_stop_continuous(dev) != 0) {
        return -5;
    }
    return 0;
}

int stm32c031_ads1299_beginner_demo(uint32_t stream_frames)
{
    ads1299_platform_hal_t hal;
    ads1299_mcu_port_t mcu_port;
    ads1299_port_t port;
    ads1299_t dev;

    stm32c031_example_log("ADS1299 STM32C031 beginner demo\r\n");

    if (stm32c031_ads1299_make_hal(&hal) != 0) {
        stm32c031_example_log("ERR platform HAL config\r\n");
        return -10;
    }
    if (ads1299_mcu_port_init(&mcu_port, &hal) != 0) {
        stm32c031_example_log("ERR ADS1299 port init\r\n");
        return -11;
    }
    port = ads1299_mcu_make_port(&mcu_port);
    if (ads1299_init(&dev, &port) != ADS1299_OK) {
        stm32c031_example_log("ERR ADS1299 core init\r\n");
        return -12;
    }

    if (demo_probe(&dev) != 0) {
        return -20;
    }
    if (demo_internal_test(&dev) != 0) {
        return -30;
    }
    if (demo_input_short(&dev) != 0) {
        return -40;
    }
    if (demo_configure_eeg_250sps(&dev) != 0) {
        return -50;
    }
    if (demo_stream(&dev, stream_frames) != 0) {
        return -60;
    }

    stm32c031_example_log("OK beginner demo complete\r\n");
    return 0;
}
