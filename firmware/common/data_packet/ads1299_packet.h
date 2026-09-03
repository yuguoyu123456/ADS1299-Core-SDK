#ifndef ADS1299_PACKET_H
#define ADS1299_PACKET_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ADS1299_PACKET_SYNC0       0xA5u
#define ADS1299_PACKET_SYNC1       0x5Au
#define ADS1299_PACKET_VERSION     0x00u
#define ADS1299_PACKET_CHANNELS    8u
#define ADS1299_PACKET_STATUS_BYTES 3u
#define ADS1299_PACKET_SIZE        49u

uint16_t ads1299_crc16_ccitt(const uint8_t *data, size_t len);

size_t ads1299_packet_encode(uint8_t *out,
                             size_t capacity,
                             uint8_t flags,
                             uint32_t sequence,
                             uint32_t timestamp_us,
                             const uint8_t status[ADS1299_PACKET_STATUS_BYTES],
                             const int32_t channels[ADS1299_PACKET_CHANNELS]);

#ifdef __cplusplus
}
#endif
#endif
