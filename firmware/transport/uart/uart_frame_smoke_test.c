#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "ads1299_packet.h"
#include "ads1299_uart_frame.h"

int main(void)
{
    uint8_t packet[ADS1299_PACKET_SIZE];
    uint8_t wire[ADS1299_UART_MAX_WIRE_SIZE];
    uint8_t rebuilt[ADS1299_PACKET_SIZE];
    uint8_t status[ADS1299_PACKET_STATUS_BYTES] = {0xC0u, 0x00u, 0x00u};
    int32_t channels[ADS1299_PACKET_CHANNELS] = {
        0, 1, -1, 0, 0x1234, -0x1234, 0, 7
    };
    ads1299_uart_decoder_t decoder;
    size_t wire_length;
    size_t i;
    int completed = 0;

    assert(ads1299_packet_encode(packet,
                                 sizeof(packet),
                                 ADS1299_PACKET_FLAG_DATA_VALID,
                                 55u,
                                 1000u,
                                 status,
                                 channels) == ADS1299_PACKET_SIZE);

    wire_length = ads1299_uart_encode_packet(packet,
                                              sizeof(packet),
                                              wire,
                                              sizeof(wire));
    assert(wire_length > 1u);
    assert(wire[wire_length - 1u] == ADS1299_UART_DELIMITER);

    ads1299_uart_decoder_init(&decoder);
    memset(rebuilt, 0, sizeof(rebuilt));
    for (i = 0u; i < wire_length; ++i) {
        int rc = ads1299_uart_decoder_push(&decoder, wire[i], rebuilt);
        if (rc == 1) completed++;
        else assert(rc == 0);
    }

    assert(completed == 1);
    assert(decoder.frames_ok == 1u);
    assert(decoder.frames_bad == 0u);
    assert(memcmp(packet, rebuilt, sizeof(packet)) == 0);

    /* Corrupt one encoded byte and confirm delimiter causes a rejected frame. */
    ads1299_uart_decoder_init(&decoder);
    if (wire_length > 4u) wire[3] ^= 0x01u;
    for (i = 0u; i < wire_length; ++i) {
        int rc = ads1299_uart_decoder_push(&decoder, wire[i], rebuilt);
        if (i + 1u == wire_length) assert(rc < 0);
    }
    assert(decoder.frames_ok == 0u);
    assert(decoder.frames_bad == 1u);

    /* Empty delimiters are harmless and keep the parser synchronized. */
    assert(ads1299_uart_decoder_push(&decoder, 0u, rebuilt) == 0);

    return 0;
}
