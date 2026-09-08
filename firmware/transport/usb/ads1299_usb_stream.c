#include "ads1299_usb_stream.h"

#include <string.h>

size_t ads1299_usb_batch_encode(uint8_t *out,
                                size_t out_capacity,
                                const uint8_t *packets,
                                uint8_t packet_count)
{
    size_t total;
    size_t i;

    if (!out || !packets) return 0u;
    if (packet_count == 0u || packet_count > ADS1299_USB_BATCH_MAX_PACKETS)
        return 0u;

    total = (size_t)packet_count * ADS1299_PACKET_SIZE;
    if (out_capacity < total) return 0u;

    for (i = 0u; i < packet_count; ++i) {
        const uint8_t *packet = packets + i * ADS1299_PACKET_SIZE;
        if (ads1299_packet_validate(packet, ADS1299_PACKET_SIZE) != ADS1299_PACKET_OK)
            return 0u;
    }

    memcpy(out, packets, total);
    return total;
}

void ads1299_usb_stream_decoder_init(ads1299_usb_stream_decoder_t *state)
{
    if (state) memset(state, 0, sizeof(*state));
}

static void resync_after_bad_candidate(ads1299_usb_stream_decoder_t *state)
{
    size_t i;

    for (i = 1u; i + 1u < ADS1299_PACKET_SIZE; ++i) {
        if (state->buffer[i] == ADS1299_PACKET_SYNC0 &&
            state->buffer[i + 1u] == ADS1299_PACKET_SYNC1) {
            size_t keep = ADS1299_PACKET_SIZE - i;
            memmove(state->buffer, &state->buffer[i], keep);
            state->bytes_discarded += (uint32_t)i;
            state->length = keep;
            return;
        }
    }

    if (state->buffer[ADS1299_PACKET_SIZE - 1u] == ADS1299_PACKET_SYNC0) {
        state->buffer[0] = ADS1299_PACKET_SYNC0;
        state->bytes_discarded += (uint32_t)(ADS1299_PACKET_SIZE - 1u);
        state->length = 1u;
    } else {
        state->bytes_discarded += (uint32_t)ADS1299_PACKET_SIZE;
        state->length = 0u;
    }
}

int ads1299_usb_stream_decoder_push(ads1299_usb_stream_decoder_t *state,
                                    uint8_t byte,
                                    uint8_t packet_out[ADS1299_PACKET_SIZE])
{
    if (!state || !packet_out) return -1;

    if (state->length == 0u) {
        if (byte == ADS1299_PACKET_SYNC0) {
            state->buffer[0] = byte;
            state->length = 1u;
        } else {
            state->bytes_discarded++;
        }
        return 0;
    }

    if (state->length == 1u) {
        if (byte == ADS1299_PACKET_SYNC1) {
            state->buffer[1] = byte;
            state->length = 2u;
        } else if (byte == ADS1299_PACKET_SYNC0) {
            /* Previous sync0 was noise; current byte may start a packet. */
            state->buffer[0] = byte;
            state->bytes_discarded++;
        } else {
            state->length = 0u;
            state->bytes_discarded += 2u;
        }
        return 0;
    }

    state->buffer[state->length++] = byte;
    if (state->length < ADS1299_PACKET_SIZE)
        return 0;

    if (ads1299_packet_validate(state->buffer, ADS1299_PACKET_SIZE) == ADS1299_PACKET_OK) {
        memcpy(packet_out, state->buffer, ADS1299_PACKET_SIZE);
        state->packets_ok++;
        state->length = 0u;
        return 1;
    }

    state->packets_bad++;
    resync_after_bad_candidate(state);
    return 0;
}
