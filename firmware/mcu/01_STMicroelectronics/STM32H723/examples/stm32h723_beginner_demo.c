#include "stm32h723_beginner_demo.h"

#include <stddef.h>
#include <stdint.h>

#include "ads1299.h"
#include "ads1299_packet.h"
#include "ads1299_spi.h"

#define STM32H723_DEMO_DRDY_TIMEOUT_US 1000000u
#define STM32H723_DEMO_DRDY_POLL_US       1000u
#define STM32H723_DEMO_TEST_FRAMES            8u

extern int board_ads1299_hal(ads1299_platform_hal_t *hal);

static void demo_log(const stm32h723_ads1299_demo_io_t *io, const char *message)
{
    if (io != NULL && io->log != NULL) {
        io->log(message);
    }
}

static int wait_and_read(const stm32h723_ads1299_demo_io_t *io,
                         ads1299_t *dev,
                         ads1299_frame_t *frame)
{
    ads1299_status_t st;

    st = ads1299_wait_drdy(dev,
                           STM32H723_DEMO_DRDY_TIMEOUT_US,
                           STM32H723_DEMO_DRDY_POLL_US);
    if (st != ADS1299_OK) {
        demo_log(io, "ERR DRDY timeout/check DRDY wiring\r\n");
        return -1;
    }
    if (ads1299_read_frame_continuous(dev, frame) != ADS1299_OK) {
        demo_log(io, "ERR frame read/check SPI and CS\r\n");
        return -2;
    }
    return 0;
}

static int start_continuous(ads1299_t *dev)
{
    if (ads1299_rdatac(dev) != ADS1299_OK) {
        return -1;
    }
    if (ads1299_start(dev) != ADS1299_OK) {
        (void)ads1299_sdatac(dev);
        return -2;
    }
    return 0;
}

static int stop_continuous(ads1299_t *dev)
{
    ads1299_status_t stop_status = ads1299_stop(dev);
    ads1299_status_t sdatac_status = ads1299_sdatac(dev);

    return (stop_status == ADS1299_OK && sdatac_status == ADS1299_OK) ? 0 : -1;
}

static int capture_frames(const stm32h723_ads1299_demo_io_t *io,
                          ads1299_t *dev,
                          uint32_t count)
{
    ads1299_frame_t frame;
    uint32_t i;

    for (i = 0u; i < count; ++i) {
        if (wait_and_read(io, dev, &frame) != 0) {
            return -1;
        }
    }
    return 0;
}

static int probe_device(const stm32h723_ads1299_demo_io_t *io, ads1299_t *dev)
{
    ads1299_device_id_t id;

    if (ads1299_hardware_reset(dev) != ADS1299_OK) {
        demo_log(io, "ERR RESET/check RESET PWDN power\r\n");
        return -1;
    }
    if (ads1299_sdatac(dev) != ADS1299_OK) {
        demo_log(io, "ERR SDATAC/check SPI Mode 1 CS\r\n");
        return -2;
    }
    if (ads1299_read_device_id(dev, &id) != ADS1299_OK ||
        !id.is_ads1299_family) {
        demo_log(io, "ERR ID/not ADS1299 family; check SPI Mode 1 power clock\r\n");
        return -3;
    }

    if (id.channel_count == 8u) {
        demo_log(io, "OK ID ADS1299-8\r\n");
    } else if (id.channel_count == 6u) {
        demo_log(io, "OK ID ADS1299-6\r\n");
    } else if (id.channel_count == 4u) {
        demo_log(io, "OK ID ADS1299-4\r\n");
    } else {
        demo_log(io, "ERR ID unexpected channel count\r\n");
        return -4;
    }

    return 0;
}

static int run_internal_test(const stm32h723_ads1299_demo_io_t *io,
                             ads1299_t *dev)
{
    demo_log(io, "RUN internal-test\r\n");
    if (ads1299_sdatac(dev) != ADS1299_OK) return -1;
    if (ads1299_configure_internal_test(dev,
                                        ADS1299_GAIN_24,
                                        0,
                                        ADS1299_TEST_FREQ_FCLK_DIV_2_21) != ADS1299_OK) {
        return -2;
    }
    if (start_continuous(dev) != 0) return -3;
    if (capture_frames(io, dev, STM32H723_DEMO_TEST_FRAMES) != 0) {
        (void)stop_continuous(dev);
        return -4;
    }
    if (stop_continuous(dev) != 0) return -5;
    demo_log(io, "OK internal-test frames\r\n");
    return 0;
}

static int run_input_short(const stm32h723_ads1299_demo_io_t *io,
                           ads1299_t *dev)
{
    demo_log(io, "RUN input-short\r\n");
    if (ads1299_sdatac(dev) != ADS1299_OK) return -1;
    if (ads1299_configure_input_short_test(dev, ADS1299_GAIN_24) != ADS1299_OK) {
        return -2;
    }
    if (start_continuous(dev) != 0) return -3;
    if (capture_frames(io, dev, STM32H723_DEMO_TEST_FRAMES) != 0) {
        (void)stop_continuous(dev);
        return -4;
    }
    if (stop_continuous(dev) != 0) return -5;
    demo_log(io, "OK input-short frames\r\n");
    return 0;
}

static int configure_eeg_250sps(const stm32h723_ads1299_demo_io_t *io,
                                ads1299_t *dev)
{
    uint8_t ch;
    uint8_t channel_count = (uint8_t)ads1299_effective_variant(dev);

    demo_log(io, "RUN EEG 250 SPS config\r\n");
    if (ads1299_sdatac(dev) != ADS1299_OK) return -1;
    if (ads1299_set_data_rate(dev, ADS1299_DR_250SPS) != ADS1299_OK) return -2;

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
            return -3;
        }
    }

    demo_log(io, "OK EEG 250 SPS configured\r\n");
    return 0;
}

static int stream_packets(const stm32h723_ads1299_demo_io_t *io,
                          ads1299_t *dev,
                          uint32_t stream_frames)
{
    ads1299_frame_t frame;
    uint8_t packet[ADS1299_PACKET_SIZE];
    uint32_t sequence = 0u;

    if (stream_frames == 0u) {
        demo_log(io, "OK stream skipped (stream_frames=0)\r\n");
        return 0;
    }
    if (io == NULL || io->write == NULL || io->timestamp_us == NULL) {
        return -1;
    }

    demo_log(io, "RUN stream canonical packets\r\n");
    if (start_continuous(dev) != 0) return -2;

    while (sequence < stream_frames) {
        size_t packet_len;

        if (wait_and_read(io, dev, &frame) != 0) {
            (void)stop_continuous(dev);
            return -3;
        }

        packet_len = ads1299_packet_encode(packet,
                                           sizeof(packet),
                                           0u,
                                           sequence,
                                           io->timestamp_us(),
                                           frame.status,
                                           frame.channel);
        if (packet_len != ADS1299_PACKET_SIZE) {
            (void)stop_continuous(dev);
            return -4;
        }
        if (io->write(packet, packet_len) != 0) {
            demo_log(io, "ERR stream transport/check UART USB Ethernet path\r\n");
            (void)stop_continuous(dev);
            return -5;
        }
        ++sequence;
    }

    return stop_continuous(dev) == 0 ? 0 : -6;
}

int stm32h723_ads1299_beginner_demo(const stm32h723_ads1299_demo_io_t *io,
                                    uint32_t stream_frames)
{
    ads1299_platform_hal_t hal;
    ads1299_mcu_port_t mcu_port;
    ads1299_port_t port;
    ads1299_t dev;

    demo_log(io, "ADS1299 STM32H723 beginner demo\r\n");

    if (board_ads1299_hal(&hal) != 0) return -10;
    if (ads1299_mcu_port_init(&mcu_port, &hal) != 0) return -11;
    port = ads1299_mcu_make_port(&mcu_port);
    if (ads1299_init(&dev, &port) != ADS1299_OK) return -12;

    if (probe_device(io, &dev) != 0) return -20;
    if (run_internal_test(io, &dev) != 0) return -30;
    if (run_input_short(io, &dev) != 0) return -40;
    if (configure_eeg_250sps(io, &dev) != 0) return -50;
    if (stream_packets(io, &dev, stream_frames) != 0) return -60;

    demo_log(io, "OK beginner demo complete\r\n");
    return 0;
}
