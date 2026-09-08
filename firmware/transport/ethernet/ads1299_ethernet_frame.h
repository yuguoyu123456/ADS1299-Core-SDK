#ifndef ADS1299_ETHERNET_FRAME_H
#define ADS1299_ETHERNET_FRAME_H

#include <stddef.h>
#include <stdint.h>

#include "ads1299_packet.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ADS1299_ETH_FRAME_MAGIC0 0x41u /* A */
#define ADS1299_ETH_FRAME_MAGIC1 0x39u /* 9 */
#define ADS1299_ETH_FRAME_MAGIC2 0x45u /* E */
#define ADS1299_ETH_FRAME_MAGIC3 0x54u /* T */
#define ADS1299_ETH_FRAME_VERSION 0x00u
#define ADS1299_ETH_FRAME_HEADER_SIZE 16u
#define ADS1299_ETH_FRAME_MAX_PACKETS 16u
#define ADS1299_ETH_FRAME_MAX_SIZE \
    (ADS1299_ETH_FRAME_HEADER_SIZE + ADS1299_ETH_FRAME_MAX_PACKETS * ADS1299_PACKET_SIZE)

#define ADS1299_ETH_FRAME_OFFSET_MAGIC          0u
#define ADS1299_ETH_FRAME_OFFSET_VERSION        4u
#define ADS1299_ETH_FRAME_OFFSET_FLAGS          5u
#define ADS1299_ETH_FRAME_OFFSET_PACKET_COUNT   6u
#define ADS1299_ETH_FRAME_OFFSET_PAYLOAD_BYTES  8u
#define ADS1299_ETH_FRAME_OFFSET_BLOCK_SEQUENCE 12u

typedef struct {
    uint32_t block_sequence;
    uint8_t flags;
    uint8_t packet_count;
    uint16_t payload_bytes;
} ads1299_ethernet_frame_info_t;

/* Encodes a block of packet_count canonical 49-byte ADS1299 packets.
 * packets must point to packet_count * ADS1299_PACKET_SIZE bytes.
 * Every packet is validated before it is copied to the output frame.
 * Returns encoded frame size, or 0 on error.
 */
size_t ads1299_ethernet_frame_encode(uint8_t *out,
                                     size_t out_capacity,
                                     uint32_t block_sequence,
                                     uint8_t flags,
                                     const uint8_t *packets,
                                     uint8_t packet_count);

/* Parses and validates one complete Ethernet transport frame.
 * The returned packets pointer aliases the input frame buffer.
 * Every contained canonical ADS1299 packet is validated.
 * Returns 0 on success, negative on error.
 */
int ads1299_ethernet_frame_decode(const uint8_t *frame,
                                  size_t frame_length,
                                  ads1299_ethernet_frame_info_t *info,
                                  const uint8_t **packets);

/* Reads only the fixed header. Useful for TCP stream receivers that need to
 * know how many bytes to collect before calling the full decoder.
 */
int ads1299_ethernet_frame_peek_size(const uint8_t *header,
                                     size_t header_length,
                                     size_t *frame_size_out);

#ifdef __cplusplus
}
#endif

#endif
