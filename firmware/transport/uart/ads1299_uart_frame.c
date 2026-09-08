#include "ads1299_uart_frame.h"

#include <string.h>

static size_t cobs_encode(const uint8_t *input,
                          size_t length,
                          uint8_t *output,
                          size_t capacity)
{
    size_t read_index = 0u;
    size_t write_index = 1u;
    size_t code_index = 0u;
    uint8_t code = 1u;

    if (!input || !output || capacity == 0u) return 0u;

    while (read_index < length) {
        if (input[read_index] == 0u) {
            if (code_index >= capacity) return 0u;
            output[code_index] = code;
            code = 1u;
            code_index = write_index++;
            if (write_index > capacity) return 0u;
            read_index++;
        } else {
            if (write_index >= capacity) return 0u;
            output[write_index++] = input[read_index++];
            code++;
            if (code == 0xFFu) {
                if (code_index >= capacity) return 0u;
                output[code_index] = code;
                code = 1u;
                code_index = write_index++;
                if (write_index > capacity) return 0u;
            }
        }
    }

    if (code_index >= capacity) return 0u;
    output[code_index] = code;
    return write_index;
}

static size_t cobs_decode(const uint8_t *input,
                          size_t length,
                          uint8_t *output,
                          size_t capacity)
{
    size_t read_index = 0u;
    size_t write_index = 0u;

    if (!input || !output) return 0u;

    while (read_index < length) {
        uint8_t code = input[read_index++];
        size_t copy_count;
        size_t i;

        if (code == 0u) return 0u;
        copy_count = (size_t)code - 1u;
        if (read_index + copy_count > length) return 0u;
        if (write_index + copy_count > capacity) return 0u;

        for (i = 0u; i < copy_count; ++i)
            output[write_index++] = input[read_index++];

        if (code != 0xFFu && read_index < length) {
            if (write_index >= capacity) return 0u;
            output[write_index++] = 0u;
        }
    }

    return write_index;
}

size_t ads1299_uart_encode_packet(const uint8_t *packet,
                                  size_t packet_length,
                                  uint8_t *out,
                                  size_t out_capacity)
{
    size_t encoded;

    if (!packet || !out) return 0u;
    if (packet_length != ADS1299_PACKET_SIZE) return 0u;
    if (out_capacity < ADS1299_UART_MAX_WIRE_SIZE) return 0u;
    if (ads1299_packet_validate(packet, packet_length) != ADS1299_PACKET_OK)
        return 0u;

    encoded = cobs_encode(packet,
                          packet_length,
                          out,
                          ADS1299_UART_COBS_MAX_ENCODED);
    if (encoded == 0u || encoded + 1u > out_capacity) return 0u;

    out[encoded] = ADS1299_UART_DELIMITER;
    return encoded + 1u;
}

void ads1299_uart_decoder_init(ads1299_uart_decoder_t *state)
{
    if (state) memset(state, 0, sizeof(*state));
}

int ads1299_uart_decoder_push(ads1299_uart_decoder_t *state,
                              uint8_t byte,
                              uint8_t packet_out[ADS1299_PACKET_SIZE])
{
    uint8_t decoded[ADS1299_PACKET_SIZE];
    size_t decoded_length;

    if (!state || !packet_out) return -1;

    if (byte != ADS1299_UART_DELIMITER) {
        if (state->dropping) return 0;
        if (state->length >= sizeof(state->encoded)) {
            state->dropping = 1u;
            state->overflows++;
            return 0;
        }
        state->encoded[state->length++] = byte;
        return 0;
    }

    if (state->dropping) {
        state->dropping = 0u;
        state->length = 0u;
        state->frames_bad++;
        return -2;
    }

    if (state->length == 0u)
        return 0;

    decoded_length = cobs_decode(state->encoded,
                                 state->length,
                                 decoded,
                                 sizeof(decoded));
    state->length = 0u;

    if (decoded_length != ADS1299_PACKET_SIZE ||
        ads1299_packet_validate(decoded, decoded_length) != ADS1299_PACKET_OK) {
        state->frames_bad++;
        return -3;
    }

    memcpy(packet_out, decoded, ADS1299_PACKET_SIZE);
    state->frames_ok++;
    return 1;
}
