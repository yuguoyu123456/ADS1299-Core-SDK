#ifndef ADS1299_PACKET_H
#define ADS1299_PACKET_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ADS1299_PACKET_SYNC0        0xA5u
#define ADS1299_PACKET_SYNC1        0x5Au
#define ADS1299_PACKET_VERSION      0x00u
#define ADS1299_PACKET_CHANNELS     8u
#define ADS1299_PACKET_STATUS_BYTES 3u
#define ADS1299_PACKET_SIZE         49u

/* Canonical packet byte offsets. Keep these shared across MCU/FPGA/host ports. */
#define ADS1299_PACKET_OFFSET_SYNC0      0u
#define ADS1299_PACKET_OFFSET_SYNC1      1u
#define ADS1299_PACKET_OFFSET_VERSION    2u
#define ADS1299_PACKET_OFFSET_FLAGS      3u
#define ADS1299_PACKET_OFFSET_SEQUENCE   4u
#define ADS1299_PACKET_OFFSET_TIMESTAMP  8u
#define ADS1299_PACKET_OFFSET_STATUS     12u
#define ADS1299_PACKET_OFFSET_CHANNELS   15u
#define ADS1299_PACKET_OFFSET_CRC        47u

typedef struct {
    uint8_t version;
    uint8_t flags;
    uint32_t sequence;
    uint32_t timestamp_us;
    uint8_t status[ADS1299_PACKET_STATUS_BYTES];
    int32_t channels[ADS1299_PACKET_CHANNELS];
} ads1299_packet_decoded_t;

uint16_t ads1299_crc16_ccitt(const uint8_t *data, size_t len);

size_t ads1299_packet_encode(uint8_t *out,
                             size_t capacity,
                             uint8_t flags,
                             uint32_t sequence,
                             uint32_t timestamp_us,
                             const uint8_t status[ADS1299_PACKET_STATUS_BYTES],
                             const int32_t channels[ADS1299_PACKET_CHANNELS]);

/*
 * Validate framing, supported protocol version, and CRC of one canonical
 * 49-byte packet. Returns 1 for valid, 0 for invalid.
 */
int ads1299_packet_validate(const uint8_t *packet, size_t len);

/*
 * Validate and decode one canonical packet into host-native integer fields.
 * Returns 1 on success, 0 on invalid input/CRC/version.
 */
int ads1299_packet_decode(const uint8_t *packet,
                          size_t len,
                          ads1299_packet_decoded_t *decoded);

#ifdef __cplusplus
}
#endif
#endif
