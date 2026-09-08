#ifndef ADS1299_PACKET_H
#define ADS1299_PACKET_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Wire format (little-endian for integer fields):
 * [0..1]   sync 0xA5 0x5A
 * [2]      protocol version
 * [3]      flags
 * [4..7]   sequence
 * [8..11]  timestamp_us
 * [12..14] ADS1299 STATUS[23:0], MSB first as received from ADS1299
 * [15..46] 8 signed channels, int32 little-endian (sign-extended ADS1299 24-bit)
 * [47..48] CRC16-CCITT over bytes [2..46], little-endian on wire
 */
#define ADS1299_PACKET_SYNC0          0xA5u
#define ADS1299_PACKET_SYNC1          0x5Au
#define ADS1299_PACKET_VERSION        0x00u
#define ADS1299_PACKET_CHANNELS       8u
#define ADS1299_PACKET_STATUS_BYTES   3u
#define ADS1299_PACKET_SIZE           49u
#define ADS1299_PACKET_PAYLOAD_BYTES  45u

#define ADS1299_PACKET_OFFSET_SYNC0      0u
#define ADS1299_PACKET_OFFSET_SYNC1      1u
#define ADS1299_PACKET_OFFSET_VERSION    2u
#define ADS1299_PACKET_OFFSET_FLAGS      3u
#define ADS1299_PACKET_OFFSET_SEQUENCE   4u
#define ADS1299_PACKET_OFFSET_TIMESTAMP  8u
#define ADS1299_PACKET_OFFSET_STATUS     12u
#define ADS1299_PACKET_OFFSET_CHANNELS   15u
#define ADS1299_PACKET_OFFSET_CRC        47u

/* Suggested flags. Applications may OR in private bits if required. */
#define ADS1299_PACKET_FLAG_DATA_VALID       (1u << 0)
#define ADS1299_PACKET_FLAG_LEADOFF_ACTIVE   (1u << 1)
#define ADS1299_PACKET_FLAG_TEST_SIGNAL      (1u << 2)
#define ADS1299_PACKET_FLAG_CLOCK_EXTERNAL   (1u << 3)
#define ADS1299_PACKET_FLAG_DAISY_CHAIN      (1u << 4)
#define ADS1299_PACKET_FLAG_TIMESTAMP_VALID  (1u << 5)
#define ADS1299_PACKET_FLAG_OVERFLOW         (1u << 6)
#define ADS1299_PACKET_FLAG_RESERVED         (1u << 7)

typedef enum {
    ADS1299_PACKET_OK = 0,
    ADS1299_PACKET_ERR_ARGUMENT = -1,
    ADS1299_PACKET_ERR_SIZE = -2,
    ADS1299_PACKET_ERR_SYNC = -3,
    ADS1299_PACKET_ERR_VERSION = -4,
    ADS1299_PACKET_ERR_CRC = -5
} ads1299_packet_result_t;

typedef struct {
    uint8_t version;
    uint8_t flags;
    uint32_t sequence;
    uint32_t timestamp_us;
    uint8_t status[ADS1299_PACKET_STATUS_BYTES];
    int32_t channels[ADS1299_PACKET_CHANNELS];
    uint16_t crc;
} ads1299_packet_t;

/* CRC-16/CCITT-FALSE: poly=0x1021, init=0xFFFF, refin=false, refout=false. */
uint16_t ads1299_crc16_ccitt(const uint8_t *data, size_t len);

/* Existing encoder API. Returns ADS1299_PACKET_SIZE on success, 0 on failure. */
size_t ads1299_packet_encode(uint8_t *out,
                             size_t capacity,
                             uint8_t flags,
                             uint32_t sequence,
                             uint32_t timestamp_us,
                             const uint8_t status[ADS1299_PACKET_STATUS_BYTES],
                             const int32_t channels[ADS1299_PACKET_CHANNELS]);

/* Struct-based convenience encoder. */
size_t ads1299_packet_encode_struct(uint8_t *out,
                                    size_t capacity,
                                    const ads1299_packet_t *packet);

/* Decode + validate sync/version/CRC. */
ads1299_packet_result_t ads1299_packet_decode(const uint8_t *data,
                                               size_t len,
                                               ads1299_packet_t *out);

/* Lightweight validation when a decoded struct is not needed. */
ads1299_packet_result_t ads1299_packet_validate(const uint8_t *data, size_t len);

/* ADS1299 raw-data helpers. ADS1299 channel samples are signed 24-bit two's-complement. */
int32_t ads1299_sign_extend24(uint32_t raw24);
int32_t ads1299_decode_sample24_be(const uint8_t raw[3]);
void ads1299_encode_sample24_be(int32_t sample, uint8_t raw[3]);

/* Convert a complete ADS1299 RDATA/RDATAC frame (3 status bytes + 8*3 data bytes). */
bool ads1299_decode_raw_frame(const uint8_t *raw,
                              size_t raw_len,
                              uint8_t status[ADS1299_PACKET_STATUS_BYTES],
                              int32_t channels[ADS1299_PACKET_CHANNELS]);

/* Convert ADC code to input-referred volts.
 * vref_volts is the ADS1299 reference voltage (e.g. 4.5 V), gain is PGA gain (1..24).
 * ADS1299 full-scale code magnitude is approximately 2^23-1.
 */
double ads1299_code_to_volts(int32_t code, double vref_volts, unsigned gain);
double ads1299_code_to_microvolts(int32_t code, double vref_volts, unsigned gain);

#ifdef __cplusplus
}
#endif
#endif
