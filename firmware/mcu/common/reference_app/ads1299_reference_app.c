#include "ads1299_reference_app.h"

#include "ads1299_runtime.h"
#include "ads1299_packet.h"

ads1299_status_t ads1299_reference_app_init(
    ads1299_reference_app_t *app,
    const ads1299_reference_board_t *board) {
    if (!app || !board || !board->init || !board->make_port ||
        !board->stream_write) {
        return ADS1299_EINVAL;
    }
    if (board->init(board->user) != 0) return ADS1299_EIO;

    app->board = board;
    app->sequence = 0u;
    app->sample_period_us = 4000u; /* 250 SPS bring-up profile. */

    const ads1299_port_t port = board->make_port(board->user);
    ads1299_status_t rc = ads1299_init(&app->ads, &port);
    if (rc != ADS1299_OK) return rc;

    rc = ads1299_hardware_reset(&app->ads);
    if (rc != ADS1299_OK) return rc;
    app->ads.port.delay_us(app->ads.port.user, 1000u);

    rc = ads1299_sdatac(&app->ads);
    if (rc != ADS1299_OK) return rc;

    ads1299_device_id_t id = {0};
    rc = ads1299_read_device_id(&app->ads, &id);
    if (rc != ADS1299_OK) return rc;
    if (!id.is_ads1299_family) return ADS1299_EVERIFY;

    rc = ads1299_set_data_rate(&app->ads, ADS1299_DR_250SPS);
    if (rc != ADS1299_OK) return rc;
    rc = ads1299_configure_internal_test(&app->ads, ADS1299_GAIN_24, 0,
                                         ADS1299_TEST_FREQ_FCLK_DIV_2_21);
    if (rc != ADS1299_OK) return rc;
    rc = ads1299_rdatac(&app->ads);
    if (rc != ADS1299_OK) return rc;
    return ads1299_start(&app->ads);
}

ads1299_status_t ads1299_reference_app_step(ads1299_reference_app_t *app) {
    if (!app || !app->board || !app->board->stream_write) {
        return ADS1299_EINVAL;
    }

    ads1299_status_t rc = ads1299_wait_drdy(&app->ads, 100000u, 10u);
    if (rc != ADS1299_OK) return rc;

    ads1299_frame_t frame;
    rc = ads1299_read_frame_continuous(&app->ads, &frame);
    if (rc != ADS1299_OK) return rc;

    uint8_t packet[ADS1299_PACKET_SIZE];
    const uint32_t timestamp_us = app->sequence * app->sample_period_us;
    const size_t n = ads1299_packet_encode(packet, sizeof(packet), 0u,
                                            app->sequence, timestamp_us,
                                            frame.status, frame.channel);
    if (n != ADS1299_PACKET_SIZE) return ADS1299_EIO;
    if (app->board->stream_write(app->board->user, packet, n) != 0) {
        return ADS1299_EIO;
    }
    app->sequence++;
    return ADS1299_OK;
}
