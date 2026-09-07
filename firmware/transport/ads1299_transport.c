#include "ads1299_transport.h"

ads1299_transport_status_t ads1299_transport_send(
    const ads1299_transport_t *transport,
    const uint8_t *data,
    size_t length) {
    size_t offset = 0u;

    if (!transport || !transport->write || (!data && length != 0u)) {
        return ADS1299_TRANSPORT_INVALID_ARGUMENT;
    }

    while (offset < length) {
        size_t requested = length - offset;
        size_t written;
        if (transport->max_chunk != 0u && requested > transport->max_chunk) {
            requested = transport->max_chunk;
        }
        written = transport->write(transport->user, &data[offset], requested);
        if (written == 0u || written > requested) {
            return ADS1299_TRANSPORT_WRITE_FAILED;
        }
        offset += written;
    }

    if (transport->flush && transport->flush(transport->user) != 0) {
        return ADS1299_TRANSPORT_FLUSH_FAILED;
    }
    return ADS1299_TRANSPORT_OK;
}

ads1299_transport_status_t ads1299_transport_send_sample(
    const ads1299_transport_t *transport,
    uint8_t flags,
    uint32_t sequence,
    uint32_t timestamp_us,
    const uint8_t status[ADS1299_PACKET_STATUS_BYTES],
    const int32_t channels[ADS1299_PACKET_CHANNELS]) {
    uint8_t packet[ADS1299_PACKET_SIZE];
    const size_t length = ads1299_packet_encode(packet,
                                                sizeof(packet),
                                                flags,
                                                sequence,
                                                timestamp_us,
                                                status,
                                                channels);
    if (length == 0u) {
        return ADS1299_TRANSPORT_ENCODE_FAILED;
    }
    return ads1299_transport_send(transport, packet, length);
}
