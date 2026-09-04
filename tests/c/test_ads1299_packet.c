#include "ads1299_packet.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static const uint8_t GOLDEN_PACKET[ADS1299_PACKET_SIZE] = {
    0xA5, 0x5A, 0x00, 0x03, 0x40, 0xE2, 0x01, 0x00, 0x06, 0x12, 0x0F, 0x00,
    0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x80, 0xFF, 0x7B,
    0x00, 0x00, 0x00, 0x38, 0xFE, 0xFF, 0xFF, 0x2A, 0x00, 0x00, 0x00, 0x77,
    0x90,
};

int main(void) {
    static const uint8_t crc_check[] = "123456789";
    const uint8_t status[ADS1299_PACKET_STATUS_BYTES] = {0xC0, 0x00, 0x00};
    const int32_t channels[ADS1299_PACKET_CHANNELS] = {
        0, 1, -1, 8388607, -8388608, 123, -456, 42,
    };
    uint8_t packet[ADS1299_PACKET_SIZE];

    /* CRC-16/IBM-3740 (CRC-16/CCITT-FALSE) standard check value. */
    assert(ads1299_crc16_ccitt(crc_check, sizeof(crc_check) - 1u) == 0x29B1u);

    assert(ads1299_packet_encode(packet,
                                 sizeof(packet),
                                 3u,
                                 123456u,
                                 987654u,
                                 status,
                                 channels) == ADS1299_PACKET_SIZE);
    assert(memcmp(packet, GOLDEN_PACKET, sizeof(GOLDEN_PACKET)) == 0);

    assert(ads1299_packet_encode(packet,
                                 ADS1299_PACKET_SIZE - 1u,
                                 3u,
                                 123456u,
                                 987654u,
                                 status,
                                 channels) == 0u);
    assert(ads1299_packet_encode(NULL,
                                 sizeof(packet),
                                 3u,
                                 123456u,
                                 987654u,
                                 status,
                                 channels) == 0u);
    assert(ads1299_packet_encode(packet,
                                 sizeof(packet),
                                 3u,
                                 123456u,
                                 987654u,
                                 NULL,
                                 channels) == 0u);
    assert(ads1299_packet_encode(packet,
                                 sizeof(packet),
                                 3u,
                                 123456u,
                                 987654u,
                                 status,
                                 NULL) == 0u);

    puts("ADS1299 host packet golden-vector test passed");
    return 0;
}
