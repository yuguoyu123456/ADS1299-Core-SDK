#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "ads1299_packet.h"
#include "ads1299_usb_stream.h"

static void make_packet(uint8_t out[ADS1299_PACKET_SIZE], uint32_t sequence, int32_t base)
{
    uint8_t status[ADS1299_PACKET_STATUS_BYTES] = {0xC0u, 0x00u, 0x00u};
    int32_t channels[ADS1299_PACKET_CHANNELS];
    size_t i;

    for (i = 0u; i < ADS1299_PACKET_CHANNELS; ++i)
        channels[i] = base + (int32_t)i;

    assert(ads1299_packet_encode(out,
                                 ADS1299_PACKET_SIZE,
                                 ADS1299_PACKET_FLAG_DATA_VALID,
                                 sequence,
                                 sequence * 4000u,
                                 status,
                                 channels) == ADS1299_PACKET_SIZE);
}

int main(void)
{
    uint8_t packets[3u * ADS1299_PACKET_SIZE];
    uint8_t tx[ADS1299_USB_BATCH_MAX_BYTES];
    uint8_t rebuilt[ADS1299_PACKET_SIZE];
    ads1299_usb_stream_decoder_t decoder;
    size_t tx_length;
    size_t i;
    uint32_t recovered = 0u;

    make_packet(&packets[0u * ADS1299_PACKET_SIZE], 10u, 0);
    make_packet(&packets[1u * ADS1299_PACKET_SIZE], 11u, 100);
    make_packet(&packets[2u * ADS1299_PACKET_SIZE], 12u, 200);

    tx_length = ads1299_usb_batch_encode(tx, sizeof(tx), packets, 3u);
    assert(tx_length == sizeof(packets));

    /* Feed arbitrary byte boundaries: the decoder does not depend on USB read size. */
    ads1299_usb_stream_decoder_init(&decoder);
    for (i = 0u; i < tx_length; ++i) {
        int rc = ads1299_usb_stream_decoder_push(&decoder, tx[i], rebuilt);
        assert(rc >= 0);
        if (rc == 1) {
            assert(memcmp(rebuilt,
                          &packets[(size_t)recovered * ADS1299_PACKET_SIZE],
                          ADS1299_PACKET_SIZE) == 0);
            recovered++;
        }
    }
    assert(recovered == 3u);
    assert(decoder.packets_ok == 3u);
    assert(decoder.packets_bad == 0u);

    /* Prefix noise must be ignored until canonical sync bytes appear. */
    ads1299_usb_stream_decoder_init(&decoder);
    assert(ads1299_usb_stream_decoder_push(&decoder, 0x11u, rebuilt) == 0);
    assert(ads1299_usb_stream_decoder_push(&decoder, 0x22u, rebuilt) == 0);
    assert(decoder.bytes_discarded == 2u);
    for (i = 0u; i < ADS1299_PACKET_SIZE; ++i) {
        int rc = ads1299_usb_stream_decoder_push(&decoder, packets[i], rebuilt);
        if (i + 1u == ADS1299_PACKET_SIZE) assert(rc == 1);
    }
    assert(memcmp(rebuilt, packets, ADS1299_PACKET_SIZE) == 0);

    /* Corrupt one packet, then append a valid packet; decoder must recover. */
    ads1299_usb_stream_decoder_init(&decoder);
    memcpy(tx, packets, ADS1299_PACKET_SIZE);
    tx[20] ^= 0x01u;
    memcpy(&tx[ADS1299_PACKET_SIZE],
           &packets[ADS1299_PACKET_SIZE],
           ADS1299_PACKET_SIZE);
    recovered = 0u;
    for (i = 0u; i < 2u * ADS1299_PACKET_SIZE; ++i) {
        int rc = ads1299_usb_stream_decoder_push(&decoder, tx[i], rebuilt);
        if (rc == 1) recovered++;
    }
    assert(decoder.packets_bad >= 1u);
    assert(recovered == 1u);
    assert(memcmp(rebuilt,
                  &packets[ADS1299_PACKET_SIZE],
                  ADS1299_PACKET_SIZE) == 0);

    return 0;
}
